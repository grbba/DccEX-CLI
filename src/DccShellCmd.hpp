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
/**
 * @class DccShellCmd
  * Setup of all the commands of the shell. The commands are defined in the hpp file as JSON 
  * strings. There is one Json string per menu i.e one for the commandstation menu and one 
  * for the graph build menu
 * @note n/a
 * @author grbba
 */

#ifndef DccShellCmd_h
#define DccShellCmd_h

#include <nlohmann/json.hpp>



enum paramTypes {
    string = 2,
    integer
};

struct cmdItem
{
    std::string name;
    std::map<int8_t, std::string> paramDesc;
    std::string help;
    std::map<int8_t, std::string> paramType;  
    int8_t minParameters = 0; // calculated from the mandatory field
    int8_t maxParameters = 0; // calculated from the mandatory field  
};

class DccShellCmd
{
private:
    
    void buildMenuCommands(const std::string commands);   // parses the JSON struct recieved and creates the menuCommands map

public:

    std::map<int, std::shared_ptr<cmdItem>> menuCommands;               // holds all the menu commands after parsing before actually inserting 
                                                       // the items into the cli structure
    DccShellCmd(const std::string commands);
    ~DccShellCmd() = default;
};

#endif