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
#include <filesystem>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>
#include <asio.hpp>

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

struct arduinoBoard
{
    std::string architecture;
    std::string programmer;
    std::string part; // identifier of the processor for avrdude
};

// supported boards
const arduinoBoard avrmega = {"Mega", "stk500v2", "m2560"};
const arduinoBoard avruno = {"Uno", "arduino", "atmega328p"};
const arduinoBoard avrnano = {"Nano", "arduino", "atmega328p"}; // A verifier !!
// unowifi r2
// nano every to be added

const std::set<std::string> ctypes = {"serial", "ethernet"};
const std::set<std::string> diags = {"latch", "ack", "wifi", "ethernet", "cmd", "wit"};
const std::map<std::string, bool> onoff = {{"on", 1}, {"off", 0}};
const std::map<std::string, arduinoBoard> boardTypes = {
    {"mega", avrmega},
    {"uno", avruno},
    {"nano", avrnano}};

// Utilities

std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c)
                   { return std::toupper(c); });
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
void readJsonFile(const std::string &schema_filename, std::string *schema)
{
    std::ifstream schema_file(schema_filename);

    *schema = std::string(std::istreambuf_iterator<char>(schema_file),
                          std::istreambuf_iterator<char>());
}

// execute shell command and return the output in a string

#ifdef WIN32
#define dccPopen _popen
#define dccPclose _pclose
#else
#define dccPopen popen
#define dccPclose pclose
#endif

/**
 * @brief Executes a command at os level
 *
 * @param cmd
 * @return std::string
 */
std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&dccPclose)> pipe(dccPopen(cmd, "r"), dccPclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

/**
 * @brief send a the command to the commandstation over the active connection (serial or ethernet)
 * ! There is only one connection per type now i.e. serial / ethernet
 * ! as there can be multiple interfaces on the this should be chnaged to support multiple onnections esp for ethernet
 *
 * @param cmd The command in DCC++ EX format to be send to the commandstation
 */
void sendCmd(const std::string csCmd)
{
    DBG("Sending: {}", csCmd);
    if (DccConfig::mshield == NOT_CONFIGURED && DccConfig::setMshield == false)
    {
        auto s = fmt::format("No Motorshield has been configured. Call mshield -s <sid> first.");
        throw ShellCmdExecException(s);
        return;
    }
    // check for the active Connection
    switch (DccConfig::active)
    {
    case DCC_SERIAL:
    {
        DBG("Sending over serial");
        if (DccConfig::serial.isOpen())
        {
            DccConfig::serial.write(&csCmd);
        }
        else
        {
            auto s = fmt::format("Serial port is closed, please call open first.");
            throw ShellCmdExecException(s);
        }
        break;
    }
    case DCC_ETHERNET:
    {
        DBG("Sending over ethernet");
        if (DccConfig::ethernet.isOpen())
        {
            DccConfig::ethernet.write(&csCmd);
        }
        else
        {
            auto s = fmt::format("Network connection is closed, please call open first.");
            throw ShellCmdExecException(s);
        }
        break;
    }
    case DCC_MQTT:
    {
        DBG("Sending over mqtt");
        INFO("Sending over MQTT is not yet implemented");
        break;
    }
    case DCC_CONN_UNKOWN:
    {
        auto s = fmt::format("No active connection to the commandstation. Open serial or network connection first.");
        throw ShellCmdExecException(s);
        break;
    }
    }
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

// For testing only
// const std::string SERVER_ADDRESS("test.mosquitto.org:1883");
// const std::string CLIENT_ID("dcccli");
// const std::string TOPIC("hello");

// Executors
static void rootMqtt(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    // switch on the first letter of the keyword
    switch (params[0][0])
    {
    case 'b':
    {
        INFO("MQTT connecting to broker ...");
        DccConfig::broker.connect();
        break;
    }
    case 's':
    {
        // subscribe to a topic
        INFO("MQTT topic subcriptions are not yet implemented");
        break;
    }
    default:
    {
        auto s = fmt::format("unknown mqtt command");
        throw ShellCmdExecException(s);
        break;
    }
    }
}
/**
 * @brief switching the active connection
 * ! ERROR when switching between connections esp when returning to the ethernet connection the read loop doesn't come back live
 * ! that works for the serial with no pb ...
 *
 * @param out
 * @param cmd
 * @param params
 */
static void rootUseConnection(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    DBG("{} Setting active connection to {}", params.size(), params[0]);

    switch (params.size())
    {
    case 1:
    {
        // params shall be serial or ethernet otherwise error
        if (params[0].compare("serial") == 0)
        {
            if (DccConfig::serial.isOpen())
            {
                DccConfig::active = DCC_SERIAL;
                INFO("Connection set to serial: [{} at {} baud]", DccConfig::serial.getDevice(), DccConfig::serial.getBaud());
            }
            else
            {
                ERR("No open serial connection available.");
            }
            break;
        }
        if (params[0].compare("ethernet") == 0)
        {

            // close the connection and reopen it ....
            // DccConfig::ethernet.closeConnection();
            // DccConfig::ethernet.openConnection(DccConfig::ethernet.getIpAddress(), DccConfig::ethernet.getPort());

            if (DccConfig::ethernet.isOpen())
            {
                DccConfig::active = DCC_ETHERNET;
                INFO("Connection set to network: [{}:{}]", DccConfig::ethernet.getIpAddress(), DccConfig::ethernet.getPort());
            }
            else
            {
                ERR("No open network connection available.");
            }
            break;
        }
        // Here we don't know
        auto s = fmt::format("Unknown connection type [{}]", params[0]);
        throw ShellCmdExecException(s);
        break;
    }
    default:
    {
        auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
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

void csOpenTCP(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    std::string arduinoIP;
    std::string arduinoPort;

    switch (params.size())
    {
    case 2:
    {
        arduinoIP = params[1];
        arduinoPort = fmt::format("{}", DCC_DEFAULT_PORT);
        break;
    }
    case 3:
    {
        arduinoIP = params[1];
        arduinoPort = params[2];
        break;
    }
    default:
    {
        auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
        throw ShellCmdExecException(s);
        break;
    }
    }

    if (DccConfig::ethernet.openConnection(arduinoIP, arduinoPort))
    {
        DccConfig::active = DCC_ETHERNET; // set the active connection to ethernet; the last one wins ...

        fmt::print(fg(fmt::color::green), "Network connected to {}:{}\n", arduinoIP, arduinoPort);
        // sleep_for(8s); // let the cs reply bfore shoing the prompt again
    }
    else
    {
        ERR("Failed to open connection to {}:{} baud\n", arduinoIP, arduinoPort);
    }
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
        DccConfig::active = DCC_SERIAL; // set the active connection to serial
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

/**
 * @brief opens a connection to the CommandStation either over serial or ethernet
 * depending on the capabilities of the CommandStation ( as installed / compiled by the user )
 * Serial should always be possible ( Ethernet / WiFi / MQTT your mileage may vary. Direct
 * MQ support may not be available bc of rss constraints on the MCU with respsect
 *
 * @param out
 * @param cmd
 * @param params parameters provided with the open command
 */
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
        try
        {
            csOpenTCP(out, cmd, params);
        }
        catch (ShellCmdExecException &ex)
        {
            ERR("Failed to connect: possible reasons: wrong address, non standard port on the command station");
            throw(ex);
        }
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
    out.flush();
    sendCmd("<s>");
    sleep_for(40ms); // leave some time for the cs to reply before showing the prompt again
    out << "\n";
}

void csRead(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    // int i = 1;
    // INFO("Executing [{}]", cmd->name);
    // for (auto p : params)
    // {
    //     INFO("Parameter[{}]: {}", i, p);
    //     i++;
    // }

    int cv;
    int callback = 0;
    int callbacksub = 0;

    if (!DccConfig::isConnect)
    {
        ERR("No CommandStation connected");
        return;
    }

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
    sendCmd(csCmd);

    sleep_for(2s);
    out << '\n';
    // leave some time for the cs to reply before showing the prompt again check if diag ack is on to allow for more time
}

void csDiag(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    // int i = 1;
    // INFO("Executing [{}]", cmd->name);

    // for (auto p : params)
    // {
    //     INFO("Parameter[{}]: {}", i, p);
    //     i++;
    // }

    if (params.size() != 2)
    {
        auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
        throw ShellCmdExecException(s);
    }
    if (diags.find(params[0]) == diags.end())
    {
        auto s = fmt::format("No such diagnostic option [{}]", params[0]);
        throw ShellCmdExecException(s);
    }
    if (onoff.find(params[1]) == onoff.end())
    {
        auto s = fmt::format("Invalid boolean option [{}]", params[1]);
        throw ShellCmdExecException(s);
    }

    std::string csCmd = fmt::format("<D {} {}>", str_toupper(params[0]), str_toupper(params[1]));
    sendCmd(csCmd);
}

void csPorts(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    std::map<std::int8_t, std::string> ports;
    int8_t pn = 1;
    fmt::print("Available ports:\n");

#ifdef OS_MAC
    std::stringstream portList(exec("ls /dev/cu*"));
    std::string p;
    while (getline(portList, p, '\n'))
    {
        ports.insert({pn, p});
        fmt::print("[{}]:{}\n", pn, p);
        pn++;
    }
#endif
#ifdef OS_LINUX
    std::filesystem::path p("/dev/serial/by-id");
    std::error_code ec;

    try
    {
        if (!exists(p))
        {
            auto s = fmt::format("{} does not exist", p.generic_string());
            throw ShellCmdExecException(s);
        }
        else
        {
            for (auto de : std::filesystem::directory_iterator(p))
            {
                if (is_symlink(de.symlink_status()))
                {
                    std::filesystem::path symlink_points_at = read_symlink(de);

                    p += "/";
                    p += symlink_points_at;

                    std::filesystem::path absolute_path = std::filesystem::canonical(p);
                    fmt::print("[{}]:{}\n", pn, absolute_path.generic_string());
                    ports.insert({pn, absolute_path.generic_string()});
                    pn++;
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error &ex)
    {
        ERR("{}", ex.what());
        throw ex;
    }
#endif
#ifdef OS_WIN
    char lpTargetPath[5000]; // buffer to store the path of the COMPORTS
    bool gotPort = false;    // in case the port is not found

    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        std::string str = "COM" + std::to_string(i); // converting to COM0, COM1, COM2
        DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

        // Test the return value and error if any
        if (test != 0) // QueryDosDevice returns zero if it didn't find an object
        {
            std::cout << str << ": " << lpTargetPath << std::endl;
            gotPort = true;
        }

        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
        }
    }
#endif
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

    auto architecture = params[0]; // either nano mega or uno or ...
    auto port = params[1];

    arduinoBoard board;

    switch (params.size())
    {
    case 2:
    {
        // check for architecture parameter
        auto boardIt = boardTypes.find(architecture);
        if (boardIt == boardTypes.end())
        {
            auto s = fmt::format("Unsopported board architecture [{}]", params[0]);
            throw ShellCmdExecException(s);
            return;
        }
        else
        {
            board = boardIt->second;
        }

        if (!std::filesystem::exists(DCC_CONFIG_ROOT))
        {
            exec("mkdir cs-config");
        }

        // Fetch the distribution
        auto fetchCmd = fmt::format(DCC_FETCH, DCC_BUILD_REPO, DCC_RELEASE, board.architecture, DCC_CONFIG_ZIP);
        // INFO("Fetching: {}", fetchCmd);
        INFO("Fetching necessary files ...", fetchCmd);
        exec(fetchCmd.c_str());

// unzip it and overwrite anything ( download is 'fairly small' so that is ok for the time being)
#ifdef WIN32
        auto zipCmd = fmt::format("tar -xf -q -d {} {}", DCC_CONFIG_ROOT, DCC_CONFIG_ZIP);
#else
        auto zipCmd = fmt::format("unzip -o -q -d {} {}", DCC_CONFIG_ROOT, DCC_CONFIG_ZIP);
#endif
        // INFO("Installing: {}", zipCmd);
        INFO("Installing files ...");
        exec(zipCmd.c_str());

        // path to the binary file
        auto csBin = fmt::format("{}/Arduino{}/{}", DCC_CONFIG_ROOT, board.architecture, DCC_CSBIN);

        // add execution flag to avrdude ok for linux/macos for win to be verified
        auto chmCmd = fmt::format("chmod 700 {}/bin/avrdude", DCC_AVRDUDE_ROOT);
        exec(chmCmd.c_str());

        // construct the avrdude command for upload
        auto avrCmd = fmt::format(DCC_AVRDUDE, DCC_AVRDUDE_ROOT, board.part, DCC_AVRDUDE_ROOT, board.programmer, port, csBin);
        INFO("Uploading commandstation ...");
        exec(avrCmd.c_str());
        INFO("Uploading commandstation completed.");
        break;
    }
    case 3:
    {
        // file name is user provided for upload
        break;
    }
    default:
    {
        auto s = fmt::format("Wrong number of arguments for [{}]", cmd->name);
        throw ShellCmdExecException(s);
        break;
    }
    }
}

void csWifi(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{

    // send <+ > command to the CS as string build from the params we get
    INFO("Wifi command parameters");
    for (auto &&i : params)
    {
        INFO("{}", i);
    }
}

void csNetwork(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{

    INFO("Network command parameters");
    for (auto &&i : params)
    {
        INFO("{}", i);
    }
}

void csMshield(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{

    // INFO("Motor Shield command parameters");

    u_char cstate = 0;

    int option = -1;
    int value = -1;

    if (params.size() == 0)
    {
        ERR("No parameters provided; please type help for more information");
        return;
    }

    for (size_t i = 0; i < params.size(); i++)
    {
        auto p = params[i];

        switch (p[0])
        {
        case '-':
        {
            // got a flag or an option
            // get the option or flag
            switch (p[1])
            {
            case 's':
            {
                option = i;
                cstate = 's';
                if (params.size() - i <= 1)
                {
                    ERR("Missing parameter for option {}", p);
                    return;
                };
                break;
            }
            case 'l':
            {
                cstate = 'l';
                fmt::print("Available predefined motorshields [<sid>]:name\n");
                fmt::print("Use the sid in conjunction with the -s option to load\n");
                fmt::print("and start the corresponding motorshield on the commandstation\n");
                for (auto &&ms : MotorShields)
                {
                    fmt::print("[{}]:{}\n", ms.first, ms.second.name);
                }
                return; // we are done here even if the -s follows we just list and ignore
            }
            default:
            {
                ERR("No such flag or option: {}", p);
                return;
            }
            }
            break;
        }
        default:
        {
            switch (cstate)
            {
            case 0:
            {
                ERR("No/wrong option or flag: {}", p);
                return; // bail out in all cases
            }
            case 's':
            {
                // INFO("Verifying power status ... power is OFF");
                // parameter should be an sid i.e. a number = to one of the motorshields
                int sid = d77::from_string<int>(p);
                auto s = MotorShields.find((CsMotorShield)sid);
                if (s == MotorShields.end())
                {
                    ERR("No Motorshield defined for sid: {}", sid);
                    return;
                }

                auto x = s->second;
                auto y = x.toString();
                // INFO("MotorShield: {}", y);

                // std::string payload = fmt::format(":s:{}:x:12:", sid);
                // std::string csCmd = fmt::format("<+cli[{}]{}>", payload.length(), payload);
                std::string csCmd = fmt::format("<cli {} {}>", cstate, sid);
                // INFO("Sending {}", csCmd);
                DccConfig::setMshield = true;
                sendCmd(csCmd);
                sleep_for(1s); // let the cs reply before showing the prompt again
                DccConfig::setMshield = false;
                INFO("Motorshield [{}] is configured and operational", s->second.name);

                DccConfig::mshield = (CsMotorShield)sid;
                // p needs checking for being valid sid!! if not --> bail out
                return; // we are done even if -l follows just ignore it
            }
            default:
            {
                ERR("Unknown error at: {}", p);
            }
            }
            // if cstate == flag|init --> error (flags have no following value and we can only start with a - i.e. flag or option)
            // if cstate == s then we should get now some text for the sid i.e. not starting with - and we should find the sd in the list of allowed values
            // && send the command to the cs <+cli:s:sid> e.g. <+cli:s:0> 0 being the enum value of the Standard_motor_shield (ref to the shield section on the web site)
            // value = i;
            // in this case no need to look any further
            // back to cstate = init;
            // if option state read the following parameter
        }
        }
    }
}

void loLoadLayout(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    INFO("Loading layout: {}", params[0]);
    DccConfig::dccLayoutFile = params[0];
    DccConfig::_playout->build(params[0], DccConfig::dccSchemaFile);
}

void loLoadSchema(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    INFO("Loading schema: {}", params[0]);
}

void loUpload(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    INFO("uploading definitions: {}", params[0]);
}

void loBuild(std::ostream &out, std::shared_ptr<cmdItem> cmd, std::vector<std::string> params)
{
    INFO("Building ...");
}

void ShellCmdExec::setup()
{
    DBG("Setup command executors");
    add(1, "config", rootConfig);
    add(1, "use", rootUseConnection);
    add(1, "loglevel", rootLogLevel);
    add(1, "mqtt", rootMqtt);
    add(2, "open", csOpen);
    add(2, "read", csRead);
    add(2, "diag", csDiag);
    add(2, "status", csStatus);
    add(2, "ports", csPorts);
    add(2, "upload", csUpload);
    add(2, "wifi", csWifi);
    add(2, "network", csNetwork);
    add(2, "mshield", csMshield);
    add(3, "layout", loLoadLayout);
    add(3, "schema", loLoadSchema);
    add(3, "upload", loUpload);
    add(3, "build", loBuild);
}