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
#include <fstream>
#include <chrono>
#include <map>
#include <set>
#include <vector>

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

// DccSerial serial = DccConfig::serial; 

struct arduinoBoard {
    std::string architecture;
    std::string programmer;
    std::string part;               // identifier of the processor for avrdude
};

// supported boards
const arduinoBoard avrmega = {"Mega", "stk500v2","m2560"};
const arduinoBoard avruno = {"Uno", "arduino","atmega328p"};
const arduinoBoard avrnano = {"Nano", "arduino", "atmega328p"}; // A verifier !!
// unowifi r2
// nano every to be added

const std::set<std::string> ctypes = {"serial","ethernet"};
const std::set<std::string> diags = {"ack", "wifi", "ethernet", "cmd", "wit"};
const std::map<std::string, bool> onoff = {{"on", 1}, {"off", 0}};
const std::map<std::string, arduinoBoard> boardTypes = {
    {"mega",avrmega },
    {"uno", avruno },
    {"nano", avrnano} 
};


// Utilities

std::string str_toupper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::toupper(c); } 
                  );
    return s;
}

/**
 * @brief Reading a file containing json;
 * Usable for small files as it fills a potentially large string.
 * For larger files direct streaming into the schema validator and/or json
 * parser should be preferred
 *
 * @param schema_filename File from which to read
 * @param schema Pointer to the string which holds the files json; filled with
 * the files content
 */
void readJsonFile(const std::string &schema_filename, std::string *schema) {
  std::ifstream schema_file(schema_filename);

  *schema = std::string(std::istreambuf_iterator<char>(schema_file),
                        std::istreambuf_iterator<char>());
}

// execute shell command and return the output in a string

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
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
    Diag::setLogLevel(LOGV_INFO);
    Diag::setFileInfo(false);
    Diag::setPrintLabel(false);

    INFO("Current session configuration:\n");
    INFO("Dcc Schema file: {}", DccConfig::dccSchemaFile);
    INFO("Dcc Layout file: {}", DccConfig::dccLayoutFile);
    INFO("Current Log level: {}", Diag::getDiagMap()[DccConfig::level]);
    INFO("> Errors and Warnings will always be shown independent of the logging level set");
    INFO("Show file information in logging messages: {}", DccConfig::fileInfo);
    INFO("Executable: {}", DccConfig::getPath());

    Diag::pop();
}

void csOpenSerial(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{

    bool isOpen = false;
    int baudRate = DCC_DEFAULT_BAUDRATE;
    switch (params.size())
    {
    case 2:
    {
        try
        {
            isOpen = DccConfig::serial.openPort(params[1], baudRate);
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
            isOpen = DccConfig::serial.openPort(params[1], baudRate);
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
    if (DccConfig::serial.isOpen())
    {
        DccConfig::serial.write(&csCmd);
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
    if (DccConfig::serial.isOpen())
    {
        DccConfig::serial.write(&csCmd);
    }
    else
    {
        // ERR("Serial port is closed, please call open first.");
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
    if (DccConfig::serial.isOpen())
    {
        DccConfig::serial.write(&csCmd);
    }
    else
    {
        auto s = fmt::format("Serial port is closed, please call open first.");
        throw ShellCmdExecException(s);
    }
    // out << "\n";
}

void csPorts(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    std::stringstream portList(exec("ls /dev/cu*"));
    std::map<std::int8_t,std::string> ports;
    
    std::string p;
    int8_t pn = 1;
    fmt::print("Available ports:\n", p);
    while(getline(portList, p, '\n'))
    { 
        ports.insert({pn,p});
        fmt::print("[{}]:{}\n", pn, p);
        pn++;
    }
}

void csUpload(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{

    /**
     * @todo
     * - Bail out on wrong port early ( i.e. before download)
     * - Also if the port is not available bc used other places ...
     * - If the CSFiles for the architecture are already there don't download again 
     * except if the latest flag is set ( to be added in the options list ) 
     * - Add user provided file download 
     */

    auto architecture = params[0];  // either nano mega or uno or ...
    auto port = params[1];

    arduinoBoard board;

    switch(params.size())
    {
        case 2: {         
            // check for architecture parameter
            auto boardIt = boardTypes.find(architecture);
            if( boardIt == boardTypes.end() ) {
                    auto s = fmt::format("Unsopported board architecture [{}]", params[0]);
                    throw ShellCmdExecException(s);
                    return;
            } else {
                board = boardIt->second;
            }

            if (!std::filesystem::exists(DCC_CONFIG_ROOT)) {
                exec("mkdir cs-config");
            }
            
            // Fetch the distribution
            auto fetchCmd = fmt::format(DCC_FETCH, DCC_BUILD_REPO, DCC_RELEASE, board.architecture, DCC_CONFIG_ZIP);
            // INFO("Fetching: {}", fetchCmd);
            INFO("Fetching necessary files ...", fetchCmd);
            exec(fetchCmd.c_str());
            
            // unzip it and overwrite anything ( download is 'fairly small' so that is ok for the time being)
            auto zipCmd = fmt::format("unzip -o -q -d {} {}", DCC_CONFIG_ROOT, DCC_CONFIG_ZIP); 
            // INFO("Installing: {}", zipCmd);
            INFO("Installing files ...");
            exec(zipCmd.c_str());
           
            // path to the binary file
            auto csBin = fmt::format("{}/Arduino{}/{}", DCC_CONFIG_ROOT, board.architecture, DCC_CSBIN);
            
            // add execution flag to avrdude ok for linux/macos for win to be verified
            auto chmCmd = fmt::format("chmod 700 {}/bin/avrdude", DCC_AVRDUDE_ROOT); 
            exec(chmCmd.c_str());
            
            // construct the avrdude command for upload
            auto avrCmd = fmt::format(DCC_AVRDUDE, DCC_AVRDUDE_ROOT, board.part, DCC_AVRDUDE_ROOT, board.programmer, port, csBin );
            INFO("Uploading commandstation ...");
            exec(avrCmd.c_str());
            INFO("Uploading commandstation completed.");
            break;
        }
        case 3: {
            // file name is user provided for upload
            break;
        } 
        default:{
            auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
            throw ShellCmdExecException(s);
            break;
        }

    }
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
    add(2, "ports", csPorts);
    add(2, "upload", csUpload);
}