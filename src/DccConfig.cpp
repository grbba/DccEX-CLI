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

#include <fmt/core.h>
#include <fmt/color.h>
#include <fstream>
#include <iostream>
#include <libproc.h>
#include <unistd.h>

#include "DccConfig.hpp"
#include "DccSerial.hpp"
#include "../include/CLI11.hpp"

std::string DccConfig::path;
std::string DccConfig::mcu;
std::string DccConfig::port;
std::string DccConfig::dccLayoutFile;
std::string DccConfig::dccSchemaFile = CONFIG_DCCEX_SCHEMA;
bool DccConfig::isInteractive = CONFIG_INTERACTIVE;
bool DccConfig::isUpload = false;
bool DccConfig::isConnect = false;
bool DccConfig::fileInfo = CONFIG_FILEINFO;
int DccConfig::baud = DCC_DEFAULT_BAUDRATE;
DiagLevel DccConfig::level = LOGV_WARN; // by default show everything up to Warning level
DccSerial DccConfig::serial;

// void verboseOptionF(const std::string &s) {
//     INFO("verbose 2loglevel flag {}", s);
// }

std::function<void(const std::string&)> verboseOptionLambda = 
    [](const std::string& s) { 
        INFO("Setting logging level to {}", s); 
        DiagLevel dl = Diag::getDiagLevel(s);
        Diag::setLogLevel(dl);
    };

std::function<void(const std::int64_t)> connectionLambda = 
    [](const std::int64_t v) { 
        INFO("Connection {}", v);
        INFO("Port: {}", DccConfig::port);
        INFO("Baud: {}", DccConfig::baud);
        
        DccConfig::serial.openPort(DccConfig::port, DccConfig::baud);

        // connect here as if open called from the interactive shell
        
    };


auto DccConfig::setup(int argc, char **argv) -> int
{

    CLI::App app{"DCC++ EX Commandline Interface Help"};

    app.get_formatter()->label("REQUIRED", "(mandatory)");
    app.get_formatter()->column_width(40);

    app.add_option("-s,--schema", dccSchemaFile,
                   "path/name of the DCC++ EX Layout Schema file. If this file "
                   "is omitted we will\nlook for the the schema file in the "
                   "current directory. If not found the system will \ncontinue "
                   "but no validation of the layout will be done and this may "
                   "yield \nunpredictable results")
        ->check(CLI::ExistingFile);

    app.add_option<std::string>("-l,--layout", DccConfig::dccLayoutFile,
                                "path/name of the modelrailroad layout file")
        // ->required()
        ->check(CLI::ExistingFile);

    auto mcuOption = app.add_option<std::string>("-m,--mcu", DccConfig::mcu,
                                "set the mcu architecture can be one of uno, mega or nano ")
        ->group("Upload");

    auto portOption = app.add_option<std::string>("-p,--port", DccConfig::port,
                                "set the port to which the arduino is connected for uploading ")
        ->group("Upload")
        ->group("Connect");


    auto connectFlag = app.add_flag_function(
        "-c,--commandstation",
        connectionLambda,
        // DccConfig::isConnect,
        "Connect to the commandstation either on the serial port (-p) of the DCC++ EX"
        "commandstation.( Ethernet connection is to come)\n If -i has been specified as well, after sending, \n"
        "the interactive commandline interface will be available.")
        ->group("Connect");

    auto baudOption = app.add_option<int>("-b, --baud", DccConfig::baud,
                                "set the baud rate for serial connection. If omitted the default of 115200 is used")
        ->group("Connect");

    connectFlag->needs(portOption); // or IP adress onec thats there  


    app.add_option_function("-v,--verbose", 
                    verboseOptionLambda,
                   "Verbose settings. Can be one of [silent|info|warning|debug|trace]\n"
                   "Default is set to 'info', 'trace' will provide the most verbose output.");

    app.add_flag("-i,--interactive", DccConfig::isInteractive,
                 "Interactive mode; Opens a shell from which commands can be "
                 "issued type --help for more info");

    auto upLoadFlag = app.add_flag("-u,--upload", DccConfig::isUpload,
                 "upload the cs code to the mcu set in -m or --mcu connected to the port -p ")
        ->group("Upload");

    // auto mcuOption = app.add_option<std::string>("-m,--mcu", DccConfig::mcu,
    //                             "set the mcu architecture can be one of uno, mega or nano ")
    //     ->group("Upload")
    //     ->group("Connect");

    // auto portOption = app.add_option<std::string>("-p,--port", DccConfig::port,
    //                             "set the port to which the arduino is connected for uploading ")
    //     ->group("Upload")
    //     ->group("Connect");

     upLoadFlag->needs(mcuOption);
     upLoadFlag->needs(portOption);   

    // CLI11_PARSE(app, argc, argv);
    try
    {
        (app).parse((argc), (argv));
    }
    catch (const CLI::ParseError &e)
    {
        (app).exit(e);
        return DCC_FAILURE;
    }

    Diag::setFileInfo(fileInfo); // if not set via commandline by default set to false

    int ret;
    pid_t pid;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

    pid = getpid();
    ret = proc_pidpath(pid, pathbuf, sizeof(pathbuf));

    if (ret <= 0)
    {
        ERR("PID {}: proc_pidpath ();\n", pid);
        ERR("     {}\n", strerror(errno));
    }
    else
    {
        DccConfig::path = std::string(pathbuf);
        // printf("proc %d: %s\n", pid, pathbuf);
    }

    return DCC_SUCCESS;
}