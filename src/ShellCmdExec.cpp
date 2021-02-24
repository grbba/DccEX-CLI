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
#include <set>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "../include/lexical_cast.hpp"

#include "Diag.hpp"
#include "DccShellCmd.hpp"
#include "DccSerial.hpp"
#include "DccConfig.hpp"
#include "ShellCmdExec.hpp"


using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

std::map<std::pair<int, std::string>, _fpShellCmd> ShellCmdExec::_fMap;
DccSerial serial;
const std::set<std::string> ctypes = {"serial","ethernet"};
const std::set<std::string> diags = {"ack", "wifi", "ethernet", "cmd", "wit"};
const std::map<std::string, bool> onoff = {{"on", 1}, {"off", 0}};

// Utilities

std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::toupper(c); } 
                  );
    return s;
}

// Executors
static void rootLogLevel(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    switch (params.size())
    {
    case 1:
    {
        auto map = Diag::getDiagMapStr();
        try
        {

            auto it = map.at(params[0]);
            DccConfig::level = it; // replace ev by an observer on the Diag class
            Diag::setLogLevel(it);
        }
        catch (std::exception &e)
        {
            auto s = fmt::format("Wrong keyword");
            throw ShellCmdExecException(s);
        }
        break;
    }
    default:
    {
        auto s = fmt::format("Wrong number of arguments");
        throw ShellCmdExecException(s);
        break;
    }
    }
}

static void rootConfig(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    Diag::push();
    Diag::setLogLevel(DiagLevel::LOGV_INFO);
    Diag::setFileInfo(false);
    Diag::setPrintLabel(false);

    INFO("Current session configuration:\n");
    INFO("Dcc Schema file: {}", DccConfig::dccSchemaFile);
    INFO("Dcc Layout file: {}", DccConfig::dccLayoutFile);
    INFO("Current Log level: {}", Diag::getDiagMap()[DccConfig::level]);
    INFO(":: Errors and Warnings will always be shown independent of the logging level set");
    INFO("Show file information in logging messages: {}", DccConfig::fileInfo);

    Diag::pop();
}

void csOpenSerial(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{

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
        auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
        throw ShellCmdExecException(s);
        break;
    }
    }
    if (isOpen)
    {

        fmt::print(fg(fmt::color::green), "Serial port {} opened at {} baud\n", params[1], baudRate);
        if (params.size() == 2)
        {
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
    switch (cType)
    {
    case DCC_SERIAL:
    {
        try
        {
            csOpenSerial(out, cmd, params);
        }
        catch (ShellCmdExecException &ex)
        {
            ERR("Failed to open port: possible reasons: wrong port, wrong baud settings or port in use by another application");
            throw(ex);
        }
        break;
    }
    case DCC_ETHERNET:
    {
        auto s = fmt::format("Ethernet is not yet supported");
        throw ShellCmdExecException(s);
        break;
    }
    default:
    {
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
        auto s = fmt::format("Serial port is closed, please call open first.");
        throw ShellCmdExecException(s);
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

    int cv;
    int callback = 0;
    int callbacksub = 0;

    switch (params.size())
    {
    case 1:
    case 3:
    {
        // only CV is provided
        try
        {
            cv = d77::from_string<int>(params[0]); // type conversion
            if (cv < 1 || cv > 1024)
            {
                auto s = fmt::format("Invalid value for CV [{}] provided: Not within the range of 1 to 1024", cv);
                throw(s);
            }
        }
        catch (std::string e)
        {
            throw ShellCmdExecException(e);
        }
        catch (std::exception &e)
        {
            auto s = fmt::format("Wrong value for CV supplied: [{}] is not a valid number", params[0]);
            throw ShellCmdExecException(s);
        }

        // either is 1 or 3 params; if three
        if (params.size() == 3)
        {
            try
            {
                callback = d77::from_string<int>(params[1]); // type conversion
                callbacksub = d77::from_string<int>(params[2]);
            }
            catch (std::exception &e)
            {
                auto s = fmt::format("Wrong value callbacknum [{}] or callbacksub [{}] ", params[1], params[2]);
                throw ShellCmdExecException(s);
            }
        }
        break;
    }
    default:
    {
        auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
        throw ShellCmdExecException(s);
        break;
    }
    }

    std::string csCmd = fmt::format("<R {} {} {}>", cv, callback, callbacksub);
    INFO("Sending: {}", csCmd);
    if (serial.isOpen())
    {
        serial.write(&csCmd);
    }
    else
    {
        ERR("Serial port is closed, please call open first.");
    }
    sleep_for(2s); 
    out << '\n';
    // leave some time for the cs to reply before showing the prompt again check if diag ack is on to allow for more time
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

    if (params.size() != 2) {
        auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
        throw ShellCmdExecException(s);
    }
    if (diags.find(params[0]) == diags.end()) {
        auto s = fmt::format("No such diagnostic option [{}]", params[0]);
        throw ShellCmdExecException(s);
    }
    if (onoff.find(params[1]) == onoff.end()) {
        auto s = fmt::format("Invalid boolean option [{}]", params[1]);
        throw ShellCmdExecException(s);
    }

    std::string csCmd = fmt::format("<D {} {}>", str_toupper(params[0]), str_toupper(params[1]));
    INFO("Sending: {}", csCmd);
    if (serial.isOpen())
    {
        serial.write(&csCmd);
    }
    else
    {
        auto s = fmt::format("Serial port is closed, please call open first.");
        throw ShellCmdExecException(s);
    }
    // out << "\n";
}

void ShellCmdExec::setup()
{

    DBG("Setup command executors");
    add(1, "config", rootConfig);
    add(1, "loglevel", rootLogLevel);
    add(2, "open", csOpen);
    add(2, "read", csRead);
    add(2, "diag", csDiag);
    add(2, "status", csStatus);
}