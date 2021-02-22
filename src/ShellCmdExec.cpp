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

/*
*   @brief executes the shell commands
*
*/
#include <iostream>
#include <chrono>
#include <map>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "../include/lexical_cast.hpp"

#include "Diag.hpp"
#include "DccShellCmd.hpp"
#include "DccSerial.hpp"

#include "ShellCmdExec.hpp"

// #define WARNING(x) fmt::format(fg(fmt::color::orange) | fmt::emphasis::bold, x);
// #define ERROR(x) fmt::format(fg(fmt::color::red) | fmt::emphasis::bold, x);
// #define OK(x) fmt::format(fg(fmt::color::green) | fmt::emphasis::bold, x);
// #define INFO(x) fmt::format(fg(fmt::color::green), x);

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

std::map<std::pair<int, std::string>, _fpShellCmd> ShellCmdExec::_fMap;
DccSerial serial;

static void rootConfig(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    INFO("Printing config\n");
}

void csOpenSerial(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params) {

   bool isOpen = false;
    int baudRate = 115200;
    switch (params.size())
    {
    case 2:
    {
        try
        {
            isOpen = serial.openPort(out, params[1], baudRate);
        }
        catch (std::exception &e)
        {
            auto s = fmt::format("Failed to open serial port [{}] at [{}] baud", params[1], baudRate);
            throw ShellCmdExecException(s);
        }
        break;
    }
    case 3:
    {
        try
        {
            baudRate = d77::from_string<int>(params[2]); // type conversion
        }
        catch (std::exception &e)
        {
            auto s = fmt::format("Wrong value for baud supplied: [{}] is not a valid number", params[2]);
            throw ShellCmdExecException(s);
        }
        try
        {
            isOpen = serial.openPort(out, params[1], baudRate);

        }
        catch (std::exception &e)
        {
            auto s = fmt::format("Failed to open serial port [{}] at [{}] baud", params[1], baudRate);
            throw ShellCmdExecException(s);
        }
        break;
    }
    default:
    {
        ERR("Wrong number of arguments for [{}]", cmd->name);
        break;
    }
    }
    if (isOpen)
    {

        fmt::print(fg(fmt::color::green), "Serial port {} opened at {} baud\n", params[1], baudRate);
        if(params.size() == 2) {
            fmt::print(fg(fmt::color::orange), "Using default baud rate\n");
        }
        sleep_for(8s); // let the cs reply bfore shoing the prompt again
    }
    else
    {
        fmt::print(fg(fmt::color::red), "Failed to open serial port {} at {} baud\n");
    }
}

void csOpen(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    int cType;
    if (params[0].compare("serial") == 0)
    {
        cType = DCC_SERIAL;
    }
    else
    {
        if (params[0].compare("ethernet") == 0)
        {
            cType = DCC_ETHERNET;
        }
        else
        {
            cType = DCC_CONN_UNKOWN;
        }
    }
    switch (cType) {
        case DCC_SERIAL:{
            try {
                csOpenSerial(out, cmd, params);
            } catch (ShellCmdExecException &ex) {
                ERR("Failed to open port: possible reasons: wrong port, wrong baud settings or port in use by another application");
                throw(ex);
            }
            break;
        }
        case DCC_ETHERNET:{
            auto s = fmt::format("Ethernet is not yet supported");
            throw ShellCmdExecException(s);
            break;
        }
        default :{
            auto s = fmt::format("Unknown connection type: [{}]", params[0]);
            throw ShellCmdExecException(s);
            break;
        }
    }
}

void csStatus(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    std::string csCmd = "<s>";
    if (serial.isOpen())
    {
        serial.write(&csCmd);
    }
    else
    {
        ERR("Serial port is closed, please call open first.");
    }
    sleep_for(10ms); // leave some time for the cs to reply before showing the prompt again
    out << "\n";
}

void csRead(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    int i = 1;
    INFO("Executing [{}]", cmd->name);
    for (auto p : params)
    {
        INFO("Parameter[{}]: {}", i, p);
        i++;
    }
    /*
    std::string csCmd = fmt::format("<R {} {} {}>", cv, cbNum, cbSub);
    out << "Sending: " << cmd << '\n';
    if (serial.isOpen())
    {
        serial.write(&csCmd);
    }
    else
    {
        out << ERROR("Serial port is closed, please call open first.");
    }
    sleep_for(1s); // leave some time for the cs to reply before showing the prompt again
    out << "\n";
    */
}

void csDiag(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    int i = 1;
    INFO("Executing [{}]", cmd->name);
    for (auto p : params)
    {
        INFO("Parameter[{}]: {}", i, p);
        i++;
    }
    /*
    std::string csCmd = fmt::format("<D {} {}>", item, state);
    if (serial.isOpen())
    {
        serial.write(&csCmd);
    }
    else
    {
        out << ERROR("Serial port is closed, please call open first.");
    }
    sleep_for(1s); // leave some time for the cs to reply before showing the prompt again
    out << "\n";
    */
}

void ShellCmdExec::setup()
{

    DBG("Setup command executors");
    add(1, "config", rootConfig);
    add(2, "open", csOpen);
    add(2, "read", csRead);
    add(2, "diag", csDiag);
    add(2, "status", csStatus);
}

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