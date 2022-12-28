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
 * @brief ShellCmdConfig List of all commands to insert
 * @note n/a
 * @author grbba
 */

#ifndef ShellCmdConfig_h
#define ShellCmdConfig_h
#include <nlohmann/json.hpp>
// { "type": "string", "desc": "-password <text> | -ssid <text> | -mode [STA|AP]", "mandatory": 0 }
// Root Menu
const std::string rootMenuItems = R"(
  {
    "menuID" : 1,
    "Commands" :
    [
      {
        "name": "config",
        "params": [],
        "help": [ "Shows the configuration items for the current sessio\n" ]
      },
      {
        "name": "use",
        "params": 
        [
          { "type": "string", "desc": "serial|ethernet", "mandatory": 1 }
        ],
        "help": [ "Allows to set the active connection in case serial and a etehrnet connection have been opened.",
                  "\tThe last opened connection will be the active one\n"
                ]
      },
      {
        "name": "loglevel",
        "params": 
        [
          { "type": "string", "desc": "silent|info|trace|debug", "mandatory": 1 }
        ],
        "help": [ 
          "Sets the logging level of the commandline interface to one of the following values",
          "\t- silent: no information besides errors and warings will be shown",
          "\t- info: some more basic information will be shown",
          "\t- trace: more detailed information on the execution of various commands",
          "\t- debug: full debugging information. This can be extremly verbose use with care",
          "\tFor commandstation diagnostics use diag in the cs menu\n"
          ]
      }
    ]
  }
)";


// Commandstation Menu
const std::string csMenuItems = R"(
  {
    "menuID" : 2,
    "Commands": [
      {
        "name": "upload",
        "params": 
        [
          { "type": "string", "desc": "mega|uno|nano", "mandatory": 1 },
          { "type": "string", "desc": "serial port", "mandatory": 1 },
          { "type": "string", "desc": "file", "mandatory": 0 },
          { "type": "string", "desc": "flags", "mandatory": 0 }
        ],
        "help": [ 
            "Upload the binary command station sketch for the arduino <type> specified",
            "\tprovided by <file> connected to the <serial port>. If file is not provided",
            "\tthe system will try to load, in order, a locally available binary release and",
            "\tif that is not available, fetch the latest available binary release.\n",
            "\tFlags: ",
            "\t -l If a Commandstation binary is avaialble locally for the choosen MCU this file will be used",
            "\t    unless -l is specified which will fetch the latest available Commandstation binary\n"
        ]
      },
      {
        "name": "ports",
        "params": 
        [],
        "help": [ 
            "lists available serial ports\n"
        ]
      },
      {
        "name": "open",
        "params": 
        [
          { "type": "string", "desc": "serial|ethernet", "mandatory": 1 },
          { "type": "string", "desc": "serial port|ip address", "mandatory": 1 },
          { "type": "integer", "desc": "baud", "mandatory": 0 }
        ],
        "help": [ 
            "open <serial|ethernet> <port> <baud>; If serial indicate the used USB",
            "\tport and for ethernet indicate the IP address of the commandstation",
            "\tbaud will be ignored for ethernet and, if not specified for serial,",
            "\tthe default of 115200 will be used.\n"
        ]
      },
      {
        "name":"status",
        "params": [],
        "help" :[ "Requesting status from the commandstation\n" ]
      },
      {
        "name":"read",
        "params": [
          { "type": "integer", "desc": "cv", "mandatory": 1 },
          { "type": "integer", "desc": "callbacknum", "mandatory": 0 },
          { "type": "integer", "desc": "callbacksub", "mandatory": 0 }
        ],
        "help" :[ "Reading CV. Allowed values for cv are 1 to 1024\n" ]
      },
      {
        "name":"diag",
        "params": [
          { "type": "string", "desc": "[latch|ack|wifi|ethernet|cmd|wit]", "mandatory": 1 },
          { "type": "string", "desc": "[on|off]", "mandatory": 1 }
        ],
        "help" : [ 
          "Enable/Disbale diganostics for the commandstation for the follwing",
          "\t- latch: capture the diagnostic output to this session instead",
          "\t         of the default serial session; off will reset to serial",
          "\t- ack: capture information for decoder communication",
          "\t- wifi: capture information for WiFi connection issues",
          "\t- ethernet: capture information for ethernet connection issues",
          "\t- cmd: capture JMRI / DCC command information",
          "\t- wit: capture WiThrottle debug information",
          "\tFor commandstation diagnostics use diag in the cs menu\n"
        ]
      },
      {
        "name": "wifi",
        "params": 
        [
          { "type": "string", "desc": "-pwd <text>", "mandatory": 0 },
          { "type": "string", "desc": "-ssid <text>", "mandatory": 0 },
          { "type": "string", "desc": "-mode [STA|AP]", "mandatory": 0 },
          { "type": "string", "desc": "flags", "mandatory": 0 }
        ],
        "help": 
        [ 
            "Configures the Wifi connection of the commandstation. The parameters can be set all together in one operation",
            "\tor one by one. Setting the configuartion will first reset any existing setting on the commandstation before",
            "\tsaving the new settings.", 
            "\n\tOptions:",
            "\t-pwd:\tpassword to connect to the wifi network",
            "\t-ssid:\tssid (network name) of the wifi network to be used",
            "\t-mode:\t[STA|AP] STA refers to Station mode i.e. the commandstation will be visible on you local network",
            "\t\tidentified by the -ssid parameter. In AP or AccessPoint mode the commandstation creates its own network to which",
            "\t\tyou connect throttles or any other commandstation compatible equipment.\n", 
            "\tFlags:",
            "\t-i:\tStart the Wifi Interface on the commandstation with the stored or specified options.",
            "\tWARNING: Configurations will only be applied if there is NO POWER on main as well as prog tracks.\n"
        ]
      },
      {
        "name": "network",
        "params": 
        [
          { "type": "string", "desc": "-ip ip address", "mandatory": 0 },
          { "type": "string", "desc": "-port port", "mandatory": 0 },
          { "type": "string", "desc": "flags", "mandatory": 0 }
        ],
        "help": 
        [ 
            "Configures the ethernet shield connection of the commandstation. The parameters can be set all together in one operation",
            "\tor one by one.",
            "\n\tOptions:",
            "\t-ip:\tif specified will set the ip adress to the given ip address. This requires that your network equipment has been",
            "\t\tconfigured accordingly.", 
            "\t-port:\tif specified overrides the default port of 2560 of the command station",
            "\n\tFlags:",
            "\t-i:\tStart the network interface over the ethernet shield on the commandstation either using DHCP and the default port or using",
            "\t\tthe specified values",
            "\tWARNING: Configurations will only be applied if there is NO POWER on main as well as prog tracks.\n"
        ]
      },
      {
        "name": "mshield",
        "params": 
        [
          { "type": "string", "desc": "-s sid", "mandatory": 0 },
          { "type": "string", "desc": "flags", "mandatory": 0 }
        ],
        "help": 
        [ 
            "Configures the motorshield of the commandstation. The -l flag allows to list all available shields and their shield id (sid).",
            "\n\tOptions:",
            "\t-s:\tsid - specifies and starts the motor shield installed on the command station. Valid sid's can be found through the -l flag",
            "\n\tFlags:",
            "\t-l:\tlists available predefined motor shields (cf. Dcc-EX website for supported shields) ",
            "\tWARNING: Configurations will only be applied if there is NO POWER on main as well as prog tracks.\n"
        ]
      }
      ]
  }
)";
// Layout Menu
const std::string loMenuItems = R"(
{
    "menuID" : 3,
    "Commands": [ 
      {
        "name": "layout",
        "params":[
            { "type": "string", "desc": "layout file", "mandatory": 1 }
        ],
        "help": [ 
            "Load the layout description file\n"
        ]
      },
      {
        "name": "schema",
        "params": [
          { "type": "string", "desc": "schema file", "mandatory": 1 }
        ],
        "help": [ 
            "Load the schema for the layout description file for validation purposes.",
            "\tIf none is provided the system will try to find a valid schema file in the local folder",
            "\tor from the dcc-ex.com website. If none is found, validation will be skipped and",
            "\tprocessing will proceed as is albeit error detection in the layout is delayed and may",
            "\tbe more complicated\n"
        ]
      },
      {
        "name": "build",
        "params": [],
        "help": [ 
            "Builds the full directed graph and possible paths for the layout and generates all turnout commands",
            "\tto be send to the command station\n"
        ]
      },
      {
        "name": "upload",
        "params": 
        [
          { "type": "string", "desc": "all|turnouts|accesories|paths", "mandatory": 1 }
        ],
        "help": [ 
            "Uploads the selected definitions to the commandstation\n"
        ]
      }
    ]
 }
)";

#endif