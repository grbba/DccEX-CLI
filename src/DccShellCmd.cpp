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


#include <fmt/color.h>
#include <fmt/core.h>

#include "Diag.hpp"

#include "DccShellCmd.hpp"

#define HEADING(x)                                                             \
  fmt::format(fg(fmt::color::medium_turquoise) | fmt::emphasis::bold, x);
#define WARNING(x) fmt::format(fg(fmt::color::orange) | fmt::emphasis::bold, x);
#define ERROR(x) fmt::format(fg(fmt::color::red) | fmt::emphasis::bold, x);

using namespace nlohmann;

/**
 * @brief Read the commnds json / handle error and insert into the map the
 * CmdItem
 *
 * @param commands
 */
void DccShellCmd::buildMenuCommands(const std::string commands) {

  json cmds = json::parse(commands);

  auto _cmd = cmds["Commands"];

  if (_cmd == nullptr) {
    std::cout << ERROR("Commands key not found\n");
    return;
  }
  int j = 0;
  for (auto c : _cmd) {
      std::shared_ptr _ci = std::make_shared<cmdItem>();
      c.at("name").get_to(_ci.get()->name);       // store the name of the menu item
      INFO("Command name: {}\n", _ci.get()->name);
      _ci->maxParameters = c.at("params").size();
      
      // loop over all the params
      int i = 0;
      for ( auto p: c.at("params")) {
          std::cout << p["type"] << std::endl;
          _ci->paramType.insert({i, p["type"]});
          std::cout << p["desc"] << std::endl;
          _ci->paramDesc.insert({i, p["desc"]});
          if(p["mandatory"]) {
            _ci->minParameters++;
          }
          std::cout << p["mandatory"] << std::endl;
          i++;
      }
      menuCommands.insert({j, _ci});
      j++;
  }
  
}

DccShellCmd::DccShellCmd(std::string commands) { buildMenuCommands(commands); }