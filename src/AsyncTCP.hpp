/*
 * © 2021 Gregor Baues. All rights reserved.
 *
 * This is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * See the GNU General Public License for more details
 * <https://www.gnu.org/licenses/>
 */

#ifndef ASYNCTCP_H
#define	ASYNCTCP_H

#include <vector>
#include <memory>
#include <functional>

#include <asio.hpp>


/**
 * Used internally (pimpl)
 */
class AsyncTCPImpl;

class AsyncTCP: private asio::noncopyable
{
public:
    AsyncTCP();
    AsyncTCP( const std::string& ipAddress, const std::string& port );  


    void open( const std::string& ipAddress, const std::string& port );   
    bool isOpen() const;        // true if tcp socket is connected 
    bool errorStatus() const;   // true if error were found
    void close();               // close the TCP conection; throws system::system_error if any error

    /**
     * Write data asynchronously. Returns immediately.
     * \param data array of char to be sent through the serial device
     * \param size array size
     */

    void write(const char *data, size_t size);

     /**
     * Write data asynchronously. Returns immediately.
     * \param data to be sent through the serial device
     */
    void write(const std::vector<char>& data);

    /**
    * Write a string asynchronously. Returns immediately.
    * Can be used to send ASCII data to the serial device.
    * To send binary data, use write()
    * \param s string to send
    */
    void writeString(const std::string& s);

    virtual ~AsyncTCP()=0;

    static const int readBufferSize = 128; // was 512

    void read() { doRead(); };

private:

    /**
     * Callback called to start an asynchronous read operation.
     * This callback is called by the io_service in the spawned thread.
     */
    void doRead();

    /**
     * Callback called at the end of the asynchronous operation.
     * This callback is called by the io_service in the spawned thread.
     */
    void readEnd(const std::error_code& error,
        size_t bytes_transferred);

    /**
     * Callback called to start an asynchronous write operation.
     * If it is already in progress, does nothing.
     * This callback is called by the io_service in the spawned thread.
     */
    void doWrite();

    /**
     * Callback called at the end of an asynchronuous write operation,
     * if there is more data to write, restarts a new write operation.
     * This callback is called by the io_service in the spawned thread.
     */
    void writeEnd(const std::error_code& error);

    /**
     * Callback to close serial port
     */
    void doClose();

    std::shared_ptr<AsyncTCPImpl> pimpl;

protected:

    void setErrorStatus(bool e);
    void setReadCallback(const std::function<void (const char*, size_t)>& callback);
    void clearReadCallback();

};

/**
 * Asynchronous TCP connection class with read callback. User code can write data
 * from one thread, and read data will be reported through a callback called
 * from a separate thred.
 */
class CallbackAsyncTCP: public AsyncTCP
{
public:
    CallbackAsyncTCP();
    CallbackAsyncTCP(const std::string& ipAddress, const std::string& port);

    /**
     * Set the read callback, the callback will be called from a thread
     * owned by the CallbackAsyncTCP class when data arrives from the
     * serial port.
     * \param callback the receive callback
     */
    void setCallback(const std::function<void (const char*, size_t)>& callback);

    /**
     * Removes the callback. Any data received after this function call will
     * be lost.
     */
    void clearCallback();

    virtual ~CallbackAsyncTCP();
};

#endif //ASYNCTCP_H
