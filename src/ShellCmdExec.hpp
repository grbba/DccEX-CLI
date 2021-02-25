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
 * @class ShellCmdExec
 * @brief This class manages the execution of shell commands.
 * Each command has is own function and all have the same type _fShellCmd. They get inserted into a map from which the executor
 * based on the recieved input gets the function to call for execution. Each of the functions is responsible for parameter
 * checking and casting as all parameters are delivered as strings.
 * @note The command definitions in ShellCmdConfig only support integer and string as types but that will be extended to any type 
 * JSON supports
 * @author grbba
 */

#ifndef ShellCmdExec_h
#define ShellCmdExec_h

#include <map>
#include <string>
#include <exception>
#include "DccShellCmd.hpp"
#include "ShellCmdConfig.hpp"

typedef void _fShellCmd(std::ostream &, std::shared_ptr<cmdItem>, std::vector<std::string>);
typedef void (*_fpShellCmd)(std::ostream &, std::shared_ptr<cmdItem>, std::vector<std::string>);

class ShellCmdExec
{
private:
    static std::map<std::pair<int, std::string>, _fpShellCmd> _fMap;
    static void init();

public:
    static void add(int menuID, std::string name, _fpShellCmd cmdFunction)
    {
        _fMap.insert({{menuID, name}, cmdFunction});
    }

    static std::map<std::pair<int, std::string>, _fpShellCmd> *getFMap()
    {
        return &_fMap;
    }

    static void setup();

    ShellCmdExec() = default;
    ~ShellCmdExec() = default;
};


class ShellCmdExecException : public std::exception
{
private:
    bool msg = false;
    const std::string cmd;

public:

	const char * what () const throw ()
    {
        if(msg) {
    	    return cmd.c_str();
        }
        return "DCC cli command execution exception";
    }

    ShellCmdExecException(const std::string c): msg(true), cmd(c) {};
    ShellCmdExecException() = default;
    ~ShellCmdExecException() = default;
};

#endif