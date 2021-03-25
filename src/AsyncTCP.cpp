

#include "AsyncTCP.hpp"

#include <algorithm>

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <memory>
#include <asio.hpp>

#include "Diag.hpp"

using namespace std;

//
// Class AsyncTCP
//

class AsyncTCPImpl : private asio::noncopyable {

public:

  asio::io_context io;                         ///< Io context object   
  asio::ip::tcp::socket csSocket;              ///< CommandStation socket
  asio::ip::tcp::resolver resolver;

  std::thread backgroundThread;               ///< Thread that runs read/write operations
  bool open;                                  ///< True if port open
  bool error;                                 ///< Error flag
  mutable std::mutex errorMutex;              ///< Mutex for access to error

  /// Data are queued here before they go in writeBuffer
  std::vector<char> writeQueue;

  std::shared_ptr<char> writeBuffer;          ///< Data being written
  size_t writeBufferSize;                     ///< Size of writeBuffer
  std::mutex writeQueueMutex;                 ///< Mutex for access to writeQueue
  char readBuffer[AsyncTCP::readBufferSize];  ///< data being read

  /// Read complete callback
  std::function<void(const char *, size_t)> callback;

  // Constructor
  AsyncTCPImpl(): io(), csSocket(io), resolver(io), backgroundThread(), open(false), error(false) {}
};


AsyncTCP::AsyncTCP() : pimpl(new AsyncTCPImpl) {}

AsyncTCP::AsyncTCP(const std::string &ipAddress, const std::string &port) : pimpl(new AsyncTCPImpl) {
  open(ipAddress, port);
}

void AsyncTCP::open(const std::string &ipAddress, const std::string &port) {
  
  if (isOpen()) {
    close();
  }

  setErrorStatus(true); // If an exception is thrown, error_ remains true

  asio::connect(pimpl->csSocket, pimpl->resolver.resolve(ipAddress, port));
  
  // This gives some work to the io_service before it is started

  asio::post(pimpl->io.get_executor(), std::bind(&AsyncTCP::doRead, this));

  // io_context:: run should not return so the thread is active all the time ..

  asio::executor_work_guard<asio::io_context::executor_type> workGuard(pimpl->io.get_executor());

  thread t(std::bind(&asio::io_context::run, &pimpl->io));
  pimpl->backgroundThread.swap(t);
  setErrorStatus(false); // If we get here, no error
  pimpl->open = true;    // Port is now open

}

bool AsyncTCP::isOpen() const { return pimpl->open; }

bool AsyncTCP::errorStatus() const {
  lock_guard<mutex> l(pimpl->errorMutex);
  return pimpl->error;
}

void AsyncTCP::close() {
  if (!isOpen()) {
    return;
  }

  pimpl->open = false;
  asio::post(pimpl->io.get_executor(), std::bind(&AsyncTCP::doClose, this));
  pimpl->backgroundThread.join();
  pimpl->io.restart();
  
  if (errorStatus()) {
    throw(std::system_error(std::error_code(), "Error while closing the device"));
  }
}

void AsyncTCP::write(const char *data, size_t size) {
  
  {
    lock_guard<mutex> l(pimpl->writeQueueMutex);
    pimpl->writeQueue.insert(pimpl->writeQueue.end(), data, data + size);
  }
  asio::post(pimpl->io.get_executor(), std::bind(&AsyncTCP::doWrite, this));
}

void AsyncTCP::write(const std::vector<char> &data) {
  
  {
    lock_guard<mutex> l(pimpl->writeQueueMutex);
    pimpl->writeQueue.insert(pimpl->writeQueue.end(), data.begin(), data.end());
  }
  asio::post(pimpl->io.get_executor(), std::bind(&AsyncTCP::doWrite, this));
}

void AsyncTCP::writeString(const std::string &s) {
  {
    lock_guard<mutex> l(pimpl->writeQueueMutex);
    pimpl->writeQueue.insert(pimpl->writeQueue.end(), s.begin(), s.end());
  }
  asio::post(pimpl->io.get_executor(), std::bind(&AsyncTCP::doWrite, this));
}

AsyncTCP::~AsyncTCP() {
  if (isOpen()) {
    try {
      close();
    } catch (...) {
      // Don't throw from a destructor
    }
  }
}

void AsyncTCP::doRead() {
  pimpl->csSocket.async_read_some(
      asio::buffer(pimpl->readBuffer, readBufferSize),
      std::bind(&AsyncTCP::readEnd, 
                this, 
                std::placeholders::_1,    // error
                std::placeholders::_2));  // bytes_transfered
}

void AsyncTCP::readEnd(const std::error_code &error, size_t bytes_transferred) {
  if (error) {
    if (isOpen()) {
      doClose();
      setErrorStatus(true);
    }
  } else {
    if (pimpl->callback)
      pimpl->callback(pimpl->readBuffer, bytes_transferred);
    doRead();
  }
}

void AsyncTCP::doWrite() {
  // If a write operation is already in progress, do nothing

  if (pimpl->writeBuffer == 0) {
    lock_guard<mutex> l(pimpl->writeQueueMutex);
    pimpl->writeBufferSize = pimpl->writeQueue.size();
    pimpl->writeBuffer.reset(new char[pimpl->writeQueue.size()]);
    copy(pimpl->writeQueue.begin(), pimpl->writeQueue.end(),
         pimpl->writeBuffer.get());
    pimpl->writeQueue.clear();
    async_write(
        pimpl->csSocket,
        asio::buffer(pimpl->writeBuffer.get(), pimpl->writeBufferSize),
        std::bind(&AsyncTCP::writeEnd, this, std::placeholders::_1));
  }
}

void AsyncTCP::writeEnd(const std::error_code &error) {

  if (!error) {
    lock_guard<mutex> l(pimpl->writeQueueMutex);
    if (pimpl->writeQueue.empty()) {
      pimpl->writeBuffer.reset();
      pimpl->writeBufferSize = 0;

      return;
    }
    pimpl->writeBufferSize = pimpl->writeQueue.size();
    pimpl->writeBuffer.reset(new char[pimpl->writeQueue.size()]);
    copy(pimpl->writeQueue.begin(), pimpl->writeQueue.end(),
         pimpl->writeBuffer.get());
    pimpl->writeQueue.clear();
    async_write(
        pimpl->csSocket,
        asio::buffer(pimpl->writeBuffer.get(), pimpl->writeBufferSize),
        std::bind(&AsyncTCP::writeEnd, this, std::placeholders::_1));
  } else {
    setErrorStatus(true);
    doClose();
  }
}

void AsyncTCP::doClose() {

  std::error_code ec;
  pimpl->csSocket.cancel(ec);
  if (ec)
    setErrorStatus(true);
  pimpl->csSocket.close(ec);
  if (ec)
    setErrorStatus(true);
}

void AsyncTCP::setErrorStatus(bool e) {
  lock_guard<mutex> l(pimpl->errorMutex);
  pimpl->error = e;
}

void AsyncTCP::setReadCallback(const std::function<void(const char *, size_t)> &callback) {
  pimpl->callback = callback;
}

void AsyncTCP::clearReadCallback() {
  std::function<void(const char *, size_t)> empty;
  pimpl->callback.swap(empty);
}


//
// Class CallbackAsyncTCP
//

CallbackAsyncTCP::CallbackAsyncTCP() : AsyncTCP() {}

CallbackAsyncTCP::CallbackAsyncTCP(
    const std::string &ipAddress, 
    const std::string &port)
    : AsyncTCP(ipAddress, port) {}

void CallbackAsyncTCP::setCallback( const std::function<void(const char *, size_t)> &callback) {
  setReadCallback(callback);
}

void CallbackAsyncTCP::clearCallback() { clearReadCallback(); }

CallbackAsyncTCP::~CallbackAsyncTCP() { clearReadCallback(); }
