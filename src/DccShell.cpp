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

#include <chrono>
#include <fstream>
#include <fmt/core.h>


#include "../include/cli/boostasioscheduler.h"

namespace cli {
using MainScheduler = BoostAsioScheduler;
} // namespace cli

// #include "../include/cli/cli.h"
#include "../include/cli/clilocalsession.h"

#include "DccShell.hpp"
#include "DccSerial.hpp"
#include "Diag.hpp"


#define HEADING(x)                                                             \
  rang::style::bold << rang::fg::cyan << x << rang::style::reset               \
                    << rang::fg::reset
#define WARNING(x)                                                             \
  rang::style::bold << rang::fg::yellow << x << rang::style::reset              \
                    << rang::fg::reset         
#define ERROR(x)                                                             \
  rang::style::bold << rang::fg::red << x << rang::style::reset               \
                    << rang::fg::reset

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

/**
 * @brief 
 * 
 * @param rootMenu 
 */
void DccShell::buildRootMenu(cli::Menu *rootMenu) {

  rootMenu->Insert(
      "config",
      [](std::ostream &) { std::cout << "Printing config" << std::endl; },
      "Shows the configuration items for the current session");
}

/**
 * @brief 
 * 
 * @param csMenu 
 */
void DccShell::buildCsMenu(cli::Menu *csMenu) {

  auto rootMenu = std::make_unique<cli::Menu>("cli");

  csMenu->Insert(
      "open", {"serial|ethernet", "serial port|ip address", "baud"},
      [&](std::ostream &out, const std::string &type,
                const std::string &device, const int &baud) {
        if (serial.openPort(out, type, device, baud)) {
          out << rang::fg::green << "Success()\n" << rang::fg::reset;
        } else {
          out << rang::fg::red << "Failed()\n" << rang::fg::reset;
        };
        sleep_for(8s);
      },
      CMD_OPEN);

  csMenu->Insert(
      "status", {},
      [&](std::ostream &out) {
        std::string cmd = "<s>";
        if(serial.isOpen()) {
        serial.write(&cmd);
        } else {
          out << ERROR("Serial port is closed, please call open first.");
        }
        sleep_for(10ms);  // leave some time for the cs to reply before showing the prompt again
        out <<"\n";
      },
      "Requesting status from the commandstation");

   csMenu->Insert(
      "read", {"cv", "callbacknum", "callbacksub"},
      [&](std::ostream &out, const int &cv, const int &cbNum, const int &cbSub) {
        std::string cmd = fmt::format("<R {} {} {}>", cv, cbNum, cbSub);
        out << "Sending: " << cmd << '\n';
        if(serial.isOpen()) {
          serial.write(&cmd);
        } else {
          out << ERROR("Serial port is closed, please call open first.");
        }
        sleep_for(20ms);  // leave some time for the cs to reply before showing the prompt again
        out <<"\n";
      },
      "Reading CV. Allowed values for cv are 1 to 1024");
}

/**
 * @brief 
 * 
 */
void DccShell::buildMenus() {
  cli::SetColor();

  auto rootMenu = std::make_unique<cli::Menu>("DccEX");
  buildRootMenu(&*rootMenu);

  auto csMenu = std::make_unique<cli::Menu>("cs");
  buildCsMenu(&*csMenu);

  rootMenu->Insert(std::move(csMenu));

  cli::Cli cli(std::move(rootMenu));

  cli.ExitAction(
      [&](auto &out) {
        serial.closePort();
        out << "Goodbye and thanks for all the steam.\n";
        std::cout.setstate(std::ios_base::badbit);
      });

  cli.StdExceptionHandler(
      [](std::ostream &out, const std::string &cmd, const std::exception &e) {
        out << "Exception caught in cli handler: " << e.what()
            << " handling command: " << cmd << ".\n";
      });

  cli::MainScheduler scheduler;
  cli::CliLocalSession localSession(cli, scheduler, std::cout, 200);

  localSession.ExitAction([&](auto &out) // session exit action
                          {
                            out << "Closing App...\n";
                            scheduler.Stop();
                          });

  scheduler.Run();

  return;
}

/**
 * @brief 
 * 
 * @return int 
 */
auto DccShell::runShell() -> int {
  // Diag::push();
  // Diag::setLogLevel(DiagLevel::LOGV_DEBUG);

  // std::cout<<"\e[2J\e[1;1H";
  // std::cout << HEADING("Welcome to the DCC++ EX Commandline Interface\n\n");

  DBG("Run Shell");

  buildMenus();

  DBG("Shell done");
  // Diag::pop();
  return DCC_SUCCESS;
}