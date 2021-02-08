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

  #include <cli/boostasioscheduler.h>
  #include <cli/boostasioremotecli.h>
  namespace cli
  {    
      using MainScheduler = BoostAsioScheduler;
      using CliTelnetServer = BoostAsioCliTelnetServer;
  }

#include "../include/cli/cli.h"
// #include "../include/cli/clifilesession.h"
#include "../include/cli/clilocalsession.h"
#include "../include/cli/filehistorystorage.h"

#include <chrono>
#include <fstream>

#include "DccShell.hpp"
#include "Diag.hpp"

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

boost::asio::io_service DccShell::io;
boost::asio::serial_port DccShell::serial(io);

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

      // set the open flag for the reciever !

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


//////////////////////
// Building the Menus
/////////////////////

// void DccShell::buildRootMenu(std::unique_ptr<cli::Menu> & rootMenu) {
void DccShell::buildRootMenu(cli::Menu * rootMenu) {

  rootMenu->Insert(
      "config",
      [](std::ostream &) { std::cout << "Printing config" << std::endl; },
      "Shows the configuration items for the current session");
}

void DccShell::buildCsMenu(cli::Menu * csMenu) {

  csMenu->Insert(
      "config",
      [](std::ostream &) { std::cout << "Printing config" << std::endl; },
      "Shows the configuration items for the current session");

  csMenu->Insert(
      "open", {"serial|ethernet", "serial port|ip address", "baud"},
      [](std::ostream &out, const std::string &type, const std::string &port,
         const int &baud) {
        int sfd;
        // ptr to the thread started here // the port is not yet open
        if (openConnection(&serial, type, port, baud, &sfd)) {
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

  auto rootMenu = std::make_unique<cli::Menu>("DccEX");
  buildRootMenu(&*rootMenu);

  auto csMenu = std::make_unique<cli::Menu>("cs");
  buildCsMenu(&*csMenu);

  rootMenu->Insert(std::move(csMenu));

  cli::Cli cli(std::move(rootMenu));

  // // global exit action
  cli.ExitAction(
      [](auto &out) { out << "Goodbye and thanks for all the fish.\n"; });

  // std exception custom handler
  cli.StdExceptionHandler(
      [](std::ostream &out, const std::string &cmd, const std::exception &e) {
        out << "Exception caught in cli handler: " << e.what()
            << " handling command: " << cmd << ".\n";
      });

  // cli::CliFileSession input(cli);
  // input.Start();

    cli::MainScheduler scheduler;
    cli::CliLocalTerminalSession localSession(cli, scheduler, std::cout, 200);
    localSession.ExitAction(
        [&scheduler](auto& out) // session exit action
        {
            out << "Closing App...\n";
            scheduler.Stop();
        }
    );

    // setup server

    cli::CliTelnetServer server(scheduler, 5000, cli);
    // exit action for all the connections
    server.ExitAction( [](auto& out) { out << "Terminating this session...\n"; } );

    scheduler.Run();


  return;
}

// function running in the reciever thread
void dccReciever(bool *exit, boost::asio::serial_port *sp) {
  std::cout << "inital exit signal " << *exit << '\n';
  std::cout << "port is " << sp->is_open() << '\n';
  // while ( sp->is_open() == false ) {};
  //std::cout << "port is " << sp->is_open() << '\n';

  int l = 0;
  while (*exit == false) {
    l++; // std::cout << "read port\n";
  }
  std::cout << "thread exiting with " << l << " reads\n";
}

auto DccShell::runShell() -> int {
  Diag::push();
  Diag::setLogLevel(DiagLevel::LOGV_DEBUG);

  // buildMenus();

  DBG("Run Shell");
  reciever = std::thread(dccReciever, &done, &serial);
  DBG("Shell reciever thread started");
  // simulate loop of the shell
  // sleep_for(5ms);
  buildMenus();
  // once done set the exit flag to true
  done = true;
  reciever.join();
  DBG("Shell done");
  Diag::pop();
  return DCC_SUCCESS;
}