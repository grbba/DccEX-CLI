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

  #include <cli/boostasioscheduler.h>
  #include <cli/boostasioremotecli.h>
  namespace cli
  {    
      using MainScheduler = BoostAsioScheduler;
      using CliTelnetServer = BoostAsioCliTelnetServer;
  }

#include "../include/cli/cli.h"
#include "../include/cli/clilocalsession.h"
#include "../include/cli/filehistorystorage.h"

#include <chrono>
#include <fstream>

#include "DccShell.hpp"
#include "Diag.hpp"

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

boost::asio::io_context DccShell::io;
boost::asio::serial_port DccShell::serial(io);
std::thread DccShell::reciever;

#ifdef ASYNC
  BufferedAsyncSerial DccShell::asyncSerial;
#endif


// opens the connection on the serial port / connects over ethernet
#ifdef ASYNC
int DccShell::openConnection(BufferedAsyncSerial *sp, std::string type,
                   const std::string port, const int baud, int *sfd) {
#else
int DccShell::openConnection(boost::asio::serial_port *sp, std::string type,
                    const std::string port, const int baud, int *sfd) {
#endif


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

#ifdef ASYNC
      reciever = std::thread(dccReciever2, &done, &asyncSerial); 
    try {
      sp->open(port.c_str(), baudRate);
      *sfd = sp->errorStatus();
#else
      reciever = std::thread(dccReciever1, &done, &serial);  
    try {
      sp->open(port.c_str(), ec);
      sp->set_option(boost::asio::serial_port_base::baud_rate(baudRate), ec);
      *sfd = ec.value();
#endif

      reciever.detach();

      std::cout << rang::fg::magenta;
      std::cout << "\nConnected to " << port.c_str() << '\n';
      std::cout << rang::fg::reset;

      // set the open flag for the reciever !

    } catch (const boost::system::system_error &e) {
      // Diag::push();
      Diag::setLogLevel(DiagLevel::LOGV_ERROR);
      Diag::setFileInfo(false);
      ERR("Unable to open serial port %s at baud rate %d", port.c_str(),
          baudRate);
      ERR("%s - %s", e.what(), ec.message().c_str());
      // Diag::pop();
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


//////////////////////
// Building the Menus
/////////////////////

void DccShell::buildRootMenu(cli::Menu * rootMenu) {

  rootMenu->Insert(
      "config",
      [](std::ostream &) { std::cout << "Printing config" << std::endl; },
      "Shows the configuration items for the current session");
}

void DccShell::buildCsMenu(cli::Menu * csMenu) {

  csMenu->Insert(
      "open", {"serial|ethernet", "serial port|ip address", "baud"},
      [&](std::ostream &out, const std::string &type, const std::string &port,
         const int &baud) {
        int sfd;

#ifdef ASYNC
        if (openConnection(&asyncSerial, type, port, baud, &sfd)) {
#else
        if (openConnection(&serial, type, port, baud, &sfd)) {
#endif
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
}

void DccShell::buildMenus() {
  cli::SetColor();

  auto rootMenu = std::make_unique<cli::Menu>("DccEX");
  buildRootMenu(&*rootMenu);

  auto csMenu = std::make_unique<cli::Menu>("cs");
  buildCsMenu(&*csMenu);

  rootMenu->Insert(std::move(csMenu));

  cli::Cli cli(std::move(rootMenu));

  // // global exit action
  // cli.ExitAction(
  //     [&](auto &out) { 
  //       out << "Goodbye and thanks for all the fish.\n"; 
  //     });

  // std exception custom handler
  cli.StdExceptionHandler(
      [](std::ostream &out, const std::string &cmd, const std::exception &e) {
        out << "Exception caught in cli handler: " << e.what()
            << " handling command: " << cmd << ".\n";
      });
 
    cli::MainScheduler scheduler(io);
    cli::CliLocalSession localSession(cli, scheduler, std::cout, 200);

    localSession.ExitAction(
        [&scheduler](auto& out) // session exit action
        {
            done = true;
            out << "Closing App...\n";
            scheduler.Stop();
        }
    );

    scheduler.Run();

  return;
}

std::string readLine(boost::asio::serial_port *port) {
  // Reading data char by char, code is optimized for simplicity, not speed
  char c;
  std::string result;
  // DBG("Reading line");
  for (;;) {
    boost::asio::read(*port, boost::asio::buffer(&c, 1));
    // DBG("Decode char");
    switch (c) {
    case '\r':
      break;
    case '\n': {
      // DBG ("Reading done ..");
      return result;
    }
    default:
      result += c;
    }
  }
}

// std::string line;

// function running in the reciever thread
#ifdef ASYNC
void DccShell::dccReciever2(bool *exit, BufferedAsyncSerial *sp) {

  std::cout << "inital exit signal " << *exit << '\n';
  std::cout.flush();

  while ( sp->isOpen() == false ) {
    sleep_for(500ms);
  };   
  
  std::cout << "port is " << (sp->isOpen() ? "open":"closed") << '\n';

  int l = 0;
  while (*exit == false) {
    // sleep_for(1s); 
    std::string line = sp->readStringUntil("\n");
    if (line.size() > 0) { std::cout << line << '\n'; }
    std::cout.flush();
    if (*exit) {
      std::cout << "Exit signal recieved ...";
    }
    l++; // std::cout << "read port\n";
  }
  std::cout << " thread exiting with " << l << " lines reads\n";
  sp->close();
  return;
}
#else
// function running in the reciever thread
void DccShell::dccReciever1(bool *exit, boost::asio::serial_port *sp) {
  
  // std::cout << "\ninital exit signal " << *exit << '\n';

  while ( sp->is_open() == false ) {
    sleep_for(500ms);
  };   

  // std::cout << "\nport is " << (sp->is_open() ? "open":"closed") << '\n';
  std::cout << "\n";
  // int l = 0;
  while (*exit == false) {
    std::cout << rang::fg::magenta;
    std::cout << readLine(sp) << '\n';
    std::cout.flush();
    // if (*exit) {
    //   std::cout << "Exit signal recieved ...";
    //   std::cout.flush();
    // }
    // l++;
    std::cout << rang::fg::reset;
  }
  // std::cout << "\nthread exiting with " << l << " lines read\n";
  // std::cout.flush();
  sp->close();

}
#endif



auto DccShell::runShell() -> int {
  // Diag::push();
  // Diag::setLogLevel(DiagLevel::LOGV_DEBUG);

  // buildMenus();

  DBG("Run Shell");

#ifdef ASYNC  
  // reciever = std::thread(dccReciever2, &done, &asyncSerial);  
#else
  // reciever = std::thread(dccReciever1, &done, &serial);  
#endif
  
  // DBG("Shell reciever thread started");

  buildMenus();

  DBG("Shell done");
  // Diag::pop();
  return DCC_SUCCESS;
}