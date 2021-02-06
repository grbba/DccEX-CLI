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

// #include "BufferedAsyncSerial.h"
// #include <boost/asio.hpp>

#include "../include/cli/cli.h"
#include "../include/cli/clifilesession.h"
#include <fstream>

#include "DccShell.hpp"
#include "Diag.hpp"

using namespace cli;

#define DCC_SERIAL 0
#define DCC_ETHERNET 1
#define DCC_CONN_UNKOWN 2

bool shellExit = false;

void dccReciever(boost::asio::serial_port *port);

boost::asio::io_service io;
boost::asio::serial_port serialPort(io);


std::shared_ptr<std::thread>
    _spDccSerialRead; //  = std::make_shared<std::thread>(dccReciever,
                      //  &serialPort);

// using namespace cli;

void doSomething(const std::vector<std::string> arg) {
  std::cout << "send: <";
  size_t i = 0;
  // int c = arg.size();
  for (auto s : arg) {
    std::cout << s.c_str(); // << " " << it->c_str();
    if (i < arg.size() - 1) {
      std::cout << "  ";
      i++;
    }
  }
  std::cout << ">\n";
}

void received(const char *data, unsigned int len) {
  std::vector<char> v(data, data + len);
  std::cout << "recieving ...\n";
  for (unsigned int i = 0; i < v.size(); i++) {
    if (v[i] == '\n') {
      std::cout << std::endl;
    } else {
      if (v[i] < 32 || v[i] >= 0x7f)
        std::cout.put(' '); // Remove non-ascii char
      else
        std::cout.put(v[i]);
    }
  }
  std::cout.flush(); // Flush screen buffer
}

std::string readLine(boost::asio::serial_port *port) {
  // Reading data char by char, code is optimized for simplicity, not speed
  char c;
  std::string result;
  for (;;) {
    boost::asio::read(*port, boost::asio::buffer(&c, 1));
    switch (c) {
    case '\r':
      break;
    case '\n':
      return result;
    default:
      result += c;
    }
  }
}

void dccReciever(boost::asio::serial_port *port) {
  INFO("\nreciever started : %d %x %d", port->is_open(), &shellExit, shellExit);
  // std::cout << "\n" << port->is_open() << shellExit << '\n';
  if (shellExit)
    std::cout << "exit changed to true\n";
  while (shellExit == false) {

    if (shellExit)
      std::cout << "exit changed to true\n";

    std::cout << readLine(port) << '\n';
  }
  return;
}

// opens the connection on the serial port / connects over ethernet
int openConnection(boost::asio::serial_port *sp, std::string type,
                   const std::string port, const int baud, int *sfd) {

  boost::system::error_code ec;

  int baudRate = 115200;
  *sfd = -1;

  int8_t cType = -1;
  if (type.compare("serial") == 0) {
    cType = DCC_SERIAL;
  } else {
    if (type.compare("ethernet") == 0) {
      std::cout << rang::fg::red << "Ethernet is not yet supported.\n"
                << rang::fg::reset;
      cType = DCC_ETHERNET;
      return DCC_FAILURE;
    } else {
      cType = DCC_CONN_UNKOWN;
      std::cout << rang::fg::red << "Unknown connection type specified.\n"
                << rang::fg::reset;
      return DCC_FAILURE;
    }
  }

  switch (cType) {
  case DCC_SERIAL: {
    baudRate = baud;

    try {
      sp->open(port.c_str(), ec);
      sp->set_option(boost::asio::serial_port_base::baud_rate(baudRate), ec);
      *sfd = ec.value();

      std::cout << rang::fg::magenta;
      std::cout << "\nConnected to " << port.c_str() << '\n';
      std::cout << rang::fg::reset;

      // create the thread for reading - reads lines forever until closed
      // std::thread dccSerialRead(dccReciever, baud);

    } catch (const boost::system::system_error &e) {
      Diag::push();
      Diag::setLogLevel(DiagLevel::LOGV_ERROR);
      Diag::setFileInfo(false);
      ERR("Unable to open serial port %s at baud rate %d", port.c_str(),
          baudRate);
      ERR("%s - %s", e.what(), ec.message().c_str());
      Diag::pop();
      return DCC_FAILURE;
    }
    break;
  }
  case DCC_ETHERNET: {
    // connect over Ethernet
    break;
  }
  case DCC_CONN_UNKOWN: {
    // error wrong command
    break;
  }
  }
  return DCC_SUCCESS;
}
/**
 * @brief running the shell
 *
 * @return int
 */
auto DccShell::runShell() -> int {

  // SerialPort for connection

  // thread to run the reciever

  std::cout << rang::style::italic
            << "Interactive mode; Type 'help' for more information\n"
            << rang::style::reset;

  SetColor();

  CmdHandler colorCmd;
  CmdHandler nocolorCmd;

  auto rootMenu = std::make_unique<Menu>("DccEX");

  rootMenu->Insert(
      "config",
      [](std::ostream &) { std::cout << "Printing config" << std::endl; },
      "Shows the configuration items for the current session");

  rootMenu->Insert("file",
                   [](std::ostream &out, int fd) {
                     out << "file descriptor: " << fd << "\n";
                   },
                   "Print the file descriptor specified", {"file_descriptor"});
  rootMenu->Insert(
      "error", [](std::ostream &) { throw std::logic_error("Error in cmd"); },
      "Throw an exception in the command handler");

  rootMenu->Insert(
      "color",
      [](std::ostream &out) {
        out << "Colors ON\n";
        SetColor();
      },
      "Enable colors in the cli");
  rootMenu->Insert(
      "nocolor",
      [](std::ostream &out) {
        out << "Colors OFF\n";
        SetNoColor();
      },
      "Disable colors in the cli");
  rootMenu->Insert("removecmds", [&](std::ostream &) {
    colorCmd.Remove();
    nocolorCmd.Remove();
  });

  // Menu for builing the layout and graph
  auto buildMenu = std::make_unique<Menu>("build");

  buildMenu->Insert(
      "config", {"Shows the configuration items for the current session"},
      [](std::ostream &) { std::cout << "Printing config" << std::endl; },
      "Shows the configuration items for the current session");

  buildMenu->Insert(
      "load",
      [](std::ostream &out, const std::string &arg) {
        out << "Loading: " << arg << '\n';
      },
      "load <file> : loads a layout file");

  // Menu for the command station
  auto csMenu = std::make_unique<Menu>("cs");

  csMenu->Insert(
      "open", {"serial|ethernet", "serial port|ip address", "baud"},
      [](std::ostream &out, const std::string &type, const std::string &port,
         const int &baud) {
        int sfd;
        // ptr to the thread started here // the port is not yet open
        if (openConnection(&serialPort, type, port, baud, &sfd)) {
          _spDccSerialRead =
              std::make_shared<std::thread>(dccReciever, &serialPort);
          out << rang::fg::green << "Success(" << sfd << ")\n"
              << rang::fg::reset;
        } else {
          out << rang::fg::red << "Failed(" << sfd << ")\n" << rang::fg::reset;
        }
      },

      "open <serial|ethernet> <port> <baud>; If serial indicate the used USB "
      "port and for ethernet indicate the\n\t"
      "IP address of the commandstation baud will be ignored for ethernet "
      "and "
      ", if not specified for serial, \n\t"
      "the default of 115200 will be used.");

  csMenu->Insert(
      "send", {"list of DCC command parameters separated by space"},
      [](std::ostream &out, std::vector<std::string> data) {
        doSomething(data);
        out << "Success\n";
      },
      "Send a < > command to the commandstation e.g.'send R 1 1' will send "
      "<R "
      "1 1> to the commandstation.");

  csMenu->Insert(
      "bye", [](std::ostream &out) { out<< "set shellExit to true"; shellExit = true; },
      "set shellExit to true");

  //   csMenu->Insert(
  //       "send",
  //       [](std::ostream &out,const std::string arg) { doSomething(arg); out
  //       << "Success\n"; },
  //       // [](std::ostream &out, const std::vector<std::string>& args ...){
  //       doSomething( args ); }, "Send a < > command to the
  //       commandstation");

  // csMenu->Insert(
  //       "send", [](std::ostream &out, const std::string &arg) { out <<
  //       "This is a sample!\n"; }, "Print a demo string");

  rootMenu->Insert(std::move(buildMenu));
  rootMenu->Insert(std::move(csMenu));

  Cli cli(std::move(rootMenu));

  // global exit action
  cli.ExitAction([](auto &out) {
    out << "Goodbye and thanks for all the fish.\n";

    _spDccSerialRead.get()->join();
  });

  // std exception custom handler
  cli.StdExceptionHandler(
      [](std::ostream &out, const std::string &cmd, const std::exception &e) {
        out << "Exception caught in cli handler: " << e.what()
            << " handling command: " << cmd << ".\n";
      });

  CliFileSession input(cli);
  _pCli = &cli;
  _pInput = &input;

  // CliSession localSession(cli, scheduler, std::cout, 200);
  input.ExitAction([](auto &out) // session exit action
                   {
                     out << "Closing App...\n";
                     shellExit = true;
                   });

  input.Start();

  return 1;
}

// Cli cli(std::move(rootMenu));

// cli.ExitAction([](auto &out) {
//   shellExit = true;
//   INFO("Goodbye and thanks for all the fish. %x %d", &shellExit, shellExit);
//   // out << "Goodbye and thanks for all the fish.\n";
// });

// // create a cli with the given root menu and a persistent storage
// // you must pass to FileHistoryStorage the path of the history file
// // if you don't pass the second argument, the cli will use a
// // VolatileHistoryStorage object that keeps in memory the history of all the
// // sessions, until the cli is shut down. Cli cli( std::move(rootMenu),
// // std::make_unique<FileHistoryStorage>(".cli") ); global exit action
// // cli.ExitAction( [](auto& out){ out << "Goodbye and thanks for all the
// // fish.\n"; } ); std exception custom handler
// cli.StdExceptionHandler(
//     [](std::ostream &out, const std::string &cmd, const std::exception &e) {
//   out << "Exception caught in cli handler: " << e.what()
//       << " handling command: " << cmd << ".\n";
//     });

// MainScheduler scheduler;
// CliLocalSession input(cli, scheduler, std::cout, 200);

// input.ExitAction([](auto &out) // session exit action
//                  {
//   out << "Closing App...\n";
//   scheduler.Stop();
//   shellExit = true;
//   _spDccSerialRead->join();
//                  });

// CliTelnetServer server(scheduler, 5000, cli);
// // exit action for all the connections
// server.ExitAction([](auto &out) {
//   out << "Terminating this session...\n"; });

// // input.Start();
// scheduler.Run();
// // _spDccSerialRead->join();
// TRC("exiting: %d", shellExit);