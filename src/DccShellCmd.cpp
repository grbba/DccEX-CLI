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

void join(const std::vector<std::string>& v, char c, std::string& s) {

   s.clear();

   for (std::vector<std::string>::const_iterator p = v.begin();
        p != v.end(); ++p) {
      s += *p;
      if (p != v.end() - 1)
        s += c;
   }
}

/**
 * @brief Read the commnds json / handle error and insert into the map the
 * CmdItem
 *
 * @param commands
 */
void DccShellCmd::buildMenuCommands(const std::string commands) {

  json cmds = json::parse(commands);

  
  auto _menuID = cmds["menuID"];
  auto _cmd = cmds["Commands"];

  if (_cmd == nullptr) {
    std::cout << ERROR("Commands key not found\n");
    return;
  }
  int j = 0;
  for (auto c : _cmd) {
      std::shared_ptr _ci = std::make_shared<cmdItem>();
      _ci.get()->menuID = _menuID;
      _ci.get()->itemID = j;
      c.at("name").get_to(_ci.get()->name);       // store the name of the menu item
      DBG("Command name: {}\n", _ci.get()->name);
      _ci->maxParameters = c.at("params").size();
      
      // loop over all the params
      int8_t i = 0;
      for ( auto p: c.at("params")) {
          _ci->paramDesc.push_back(p["desc"]);
          if(p["mandatory"] == 1) {
            _ci->paramType.insert({i, {1, p["type"]}});
            _ci->minParameters++;
          } else {
            _ci->paramType.insert({i, {0, p["type"]}});
          }
          i++;
      }
      join(c.at("help").get<std::vector<std::string>>(), '\n', _ci.get()->help);
    
      // c.at("help").get_to(_ci.get()->help); 
      menuCommands.insert({j, _ci});
      j++;
  }
  
}

DccShellCmd::DccShellCmd(std::string commands) { buildMenuCommands(commands); }