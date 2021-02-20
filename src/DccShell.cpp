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
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "../include/cli/boostasioscheduler.h"

namespace cli
{
  using MainScheduler = BoostAsioScheduler;
} // namespace cli
#include "../include/cli/clilocalsession.h"

#include "DccShell.hpp"
#include "DccShellCmd.hpp"
#include "ShellCmdConfig.hpp"
#include "DccSerial.hpp"
#include "Diag.hpp"

#define HEADING(x) fmt::format(fg(fmt::color::medium_turquoise) | fmt::emphasis::bold, x);
#define WARNING(x) fmt::format(fg(fmt::color::orange) | fmt::emphasis::bold, x);
#define ERROR(x) fmt::format(fg(fmt::color::red) | fmt::emphasis::bold, x);

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

/**
 * @brief Command distributor; identifies the command verifies the parameters send and calls the execution accordingly
 * 
 */
void executeMenu(std::ostream &out, const std::pair<int,std::shared_ptr<cmdItem>> var, const std::vector<std::string> params)
{
  out << "execute menu item: " << var.second->name << '\n';
}

/**
 * @brief crates the menu structure for a given menu; Need to find a way to register the executor somehow here... 
 */
void DccShell::buildMenuCommands(cli::Menu *menu, DccShellCmd *menuItems) {

  for (auto var : menuItems->menuCommands)
  {
    menu->Insert(
        var.second->name,                                         // Name
        var.second->paramDesc,                                    // description as many strings as parameters
        [=](std::ostream &out, std::vector<std::string> params) { // lambda to execute for this menu item
          executeMenu(out, var, params);
        },
        var.second->help); // helptext for the menu item
  }

}
/**
 * @brief 
 * 
 */
void DccShell::buildMenus()
{

  cli::SetColor();

  // Root Menu
  auto rootMenu = std::make_unique<cli::Menu>("DccEX");
  DccShellCmd rootCmdMenu(rootMenuItems);
  buildMenuCommands(&*rootMenu, &rootCmdMenu);

  // CommandStation sub menu
  auto csMenu = std::make_unique<cli::Menu>("cs"); // make a new cli menu
  DccShellCmd csCmdMenu(csMenuItems);              // constructs the menuItems
  buildMenuCommands(&*csMenu, &csCmdMenu);
  rootMenu->Insert(std::move(csMenu));             // attach the submenu to the root menu

  // Graph/Build sub menu


  // attach the menu structure to the cli
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
auto DccShell::runShell() -> int
{

  DBG("Run Shell");

  buildMenus();

  DBG("Shell done");
  return DCC_SUCCESS;
}

// ====== Stash

/*
void DccShell::buildCsMenu(cli::Menu *csMenu)
{
  csMenu->Insert(
      "open", {"serial|ethernet", "serial port|ip address", "baud"},
      [&](std::ostream &out, const std::string &type,
          const std::string &device, const int &baud) {
        if (serial.openPort(out, type, device, baud))
        {
          out << rang::fg::green << "Success()\n"
              << rang::fg::reset;
        }
        else
        {
          out << rang::fg::red << "Failed()\n"
              << rang::fg::reset;
        };
        sleep_for(8s);
      },
      CMD_OPEN);

  csMenu->Insert(
      "status", {},
      [&](std::ostream &out) {
        std::string cmd = "<s>";
        if (serial.isOpen())
        {
          serial.write(&cmd);
        }
        else
        {
          out << ERROR("Serial port is closed, please call open first.");
        }
        sleep_for(10ms); // leave some time for the cs to reply before showing the prompt again
        out << "\n";
      },
      "Requesting status from the commandstation");

  csMenu->Insert(
      "read", {"cv", "callbacknum", "callbacksub"},
      [&](std::ostream &out, const int &cv, const int &cbNum, const int &cbSub) {
        std::string cmd = fmt::format("<R {} {} {}>", cv, cbNum, cbSub);
        out << "Sending: " << cmd << '\n';
        if (serial.isOpen())
        {
          serial.write(&cmd);
        }
        else
        {
          out << ERROR("Serial port is closed, please call open first.");
        }
        sleep_for(1s); // leave some time for the cs to reply before showing the prompt again
        out << "\n";
      },
      "Reading CV. Allowed values for cv are 1 to 1024");

  csMenu->Insert(
      "diag", {"[ack|wifi|ethernet|cmd|wit]", "[on|off]"},
      [&](std::ostream &out, const std::string &item, const std::string &state) {
        std::string cmd = fmt::format("<D {} {}>", item, state);
        if (serial.isOpen())
        {
          serial.write(&cmd);
        }
        else
        {
          out << ERROR("Serial port is closed, please call open first.");
        }
        sleep_for(1s); // leave some time for the cs to reply before showing the prompt again
        out << "\n";
      },
      "Enable/Disbale diganostics for the commandstation");

  csMenu->Insert(
      mi,                                                       // Name
      midesc,                                                   // description as many strings as parameters
      [&](std::ostream &out, std::vector<std::string> params) { // lambda to execute for this menu item
        executeMenu(out, mi, params);
      },
      mihelp); // helptext for the menu item
}
*/

    /*
    fmt::print("Command: {}\n", var.second->name);
    std::cout << "Parameters : { ";
    for (auto pd : var.second->paramDesc)
    {
      fmt::print("{} ", pd);
    }
    std::cout << " }\n";
    std::cout << "Types : { ";
    for (auto pt : var.second->paramType)
    {
      fmt::print("{}:{} ", pt.second.first, pt.second.second);
    }
    std::cout << " }\n";
    fmt::print("Help: {}\n", var.second->help);
    */

/**
 * @brief 
 * 
 * @param rootMenu 
 */
/*
void DccShell::buildRootMenu(cli::Menu *rootMenu)
{

  rootMenu->Insert(
      "config",
      [](std::ostream &) { std::cout << "Printing config" << std::endl; },
      "Shows the configuration items for the current session");
}
*/