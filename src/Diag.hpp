/*
 * © 2020 Gregor Baues. All rights reserved.
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
 * @class Diag
 * @brief This class manages the diagnostic outputs, logging etc.
 * Some more text about this class
 * @note A note for doxy
 * @author grbba
 */

#ifndef Diag_h
#define Diag_h

#include <iostream>
#include <stack>
#include <map>

#include <fmt/core.h>
#include <fmt/color.h>
#include <spdlog/spdlog.h>

#define DCC_SUCCESS 1
#define DCC_FAILURE 0

typedef spdlog::level::level_enum DiagLevel;

#define INFO spdlog::info
#define ERR spdlog::error
#define WARN spdlog::warn
#define DBG spdlog::debug
#define TRC spdlog::trace // highest logging level
#define CRITICAL spdlog::critical // This is always shown excpet when the log level is set to off


#define LOGV_SILENT DiagLevel::off
#define LOGV_WARN DiagLevel::warn
#define LOGV_ERROR DiagLevel::err
#define LOGV_INFO DiagLevel::info
#define LOGV_TRACE DiagLevel::trace
#define LOGV_DEBUG DiagLevel::debug

#define CLI_INFO fmt::print("[");fmt::print(fg(fmt::color::medium_turquoise),"DccCli");fmt::print("] "); 

struct DiagConfig
{
  DiagLevel _nLogLevel;
  int _nInfoLevel; // for future use
  bool fileInfo;
  bool println;
  bool printLabel;

  DiagConfig() = default;
  ~DiagConfig() = default;
};

class Diag
{
private:
  static spdlog::level::level_enum _nLogLevel;

  static bool fileInfo;
  static bool println;
  static bool printLabel;
  static const std::map<DiagLevel, std::string> diagMap;
  static const std::map<std::string, DiagLevel> diagMapStr;
  static std::stack<DiagConfig *> config;

public:
  static auto getDiagMap() -> std::map<DiagLevel, std::string>
  {
    return diagMap;
  }

  static const std::string *getDiagLevelName(DiagLevel level);

  static auto getDiagMapStr() -> std::map<std::string, DiagLevel>
  {
    return diagMapStr;
  }
  
  static const DiagLevel getDiagLevel(std::string level);

  static void setLogLevel(spdlog::level::level_enum value) { 
    _nLogLevel = value; 
    spdlog::set_level(value); 
  }

  static spdlog::level::level_enum getLogLevel() { return _nLogLevel; }

  static void setFileInfo(bool value) { fileInfo = value; }
  static bool getFileInfo() { return fileInfo; }

  static void setPrintln(bool value) { println = value; }
  static bool getPrintln() { return println; }

  static void setPrintLabel(bool value) { printLabel = value; }
  static bool getPrintLabel() { return printLabel; }

  static void push(); // pushes a Diag Config onto the stack
  static void pop();  // pops the last diagConfig from the stack and
                      // reinstatiates its values;

  Diag() = default;
  ~Diag() = default;
};

#endif