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

// #include "../include/formatter.h"
// #include "../include/rang.hpp"

#include <fmt/core.h>
#include <fmt/color.h>
#include <iostream>
#include <stack>
#include <map>

#define DCC_SUCCESS 1
#define DCC_FAILURE 0

enum class DiagLevel {
  LOGV_SILENT,
  LOGV_INFO,
  LOGV_WARN,
  LOGV_ERROR,
  LOGV_TRACE,
  LOGV_DEBUG
};

struct DiagConfig {
  DiagLevel _nLogLevel;
  int _nInfoLevel; // for future use
  bool fileInfo;
  bool println;
  bool printLabel;

  DiagConfig() = default;
  ~DiagConfig() = default;
};

class Diag {
private:
  static DiagLevel _nLogLevel;
  static int _nInfoLevel; // for future use
  static bool fileInfo;
  static bool println;
  static bool printLabel;
  static const std::map<std::string, DiagLevel> diagMap;
  static std::stack<DiagConfig *> config;

public:
  static auto getDiagMap() -> std::map<std::string, DiagLevel> {
    return diagMap;
  }

  static void setLogLevel(DiagLevel value) { _nLogLevel = value; }

  static DiagLevel getLogLevel() { return _nLogLevel; }

  static void setInfoLevel(int value) { _nInfoLevel = value; }

  static int getInfoLevel() { return _nInfoLevel; }

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

#define EH_DW(code)                                                            \
  do {                                                                         \
    code                                                                       \
  } while (0) // wraps in a do while(0) so that the syntax is correct.

// enable/ disbale the loglevel at runtime everything up to the compile time
// level will be shown the rest not !
#define EH_IFLL(LL, code)                                                      \
  if (Diag::getLogLevel() >= LL) {                                             \
    code                                                                       \
  }
#define EH_IFIL(IL, code)                                                      \
  if (Diag::getInfoLevel() >= IL) {                                            \
    code                                                                       \
  }

#

// #define DIAG(message...) std::cout << string::formatter(message);
// #define DIAG(message...) fmt::print(fg(fmt::color::medium_turquoise), message);
#define DIAG(message...) fmt::print(message);
#define DIAGWARN(message...) fmt::print(fg(fmt::color::orange), message);
#define DIAGERR(message...) fmt::print(fg(fmt::color::red), message);

#ifndef DEBUG
#define DEBUG
#ifndef LOGLEVEL
#define LOGLEVEL 5 // = DiagLevel::LOGV_DEBUG
// compile time level by default up to error can be overridden at
// compiletime with a -D flag
#endif
#endif

// #define LOGV_DEBUG 5
// #define LOGV_TRACE 4
// #define LOGV_ERROR 3
// #define LOGV_WARN 2
// #define LOGV_INFO 1
// #define LOGV_SILENT 0

#define LOGV_INFO_MSG(message...)                                              \
  if (Diag::getPrintLabel())                                                   \
    DIAG("::[INF]:");                                                          \
  if (Diag::getFileInfo())                                                     \
    DIAG("{}:{} : ", __FILE__, __LINE__);                                      \
  DIAG(message);                                                               \
  if (Diag::getPrintln())                                                      \
    DIAG("\n");

#define LOGV_WARN_MSG(message...)                                              \
  DIAGWARN("::[WRN]:");                                                            \
  if (Diag::getFileInfo())                                                     \
    DIAGWARN("{}:{} : ", __FILE__, __LINE__);                                      \
  DIAGWARN(message);                                                               \
  if (Diag::getPrintln())                                                      \
    DIAG("\n");                                                                

#define LOGV_ERROR_MSG(message...)                                             \
  if (Diag::getFileInfo())                                                     \
    DIAGERR("::[ERR]:{}:{} : ", __FILE__, __LINE__);                              \
  DIAGERR(message);                                                               \
  DIAG("\n");  

#define LOGV_TRACE_MSG(message...)                                             \
  DIAG("::[TRC]:{}:{} : ", __FILE__, __LINE__);                                \
  DIAG(message);                                                               \
  DIAG("\n");
#define LOGV_DEBUG_MSG(message...)                                             \
  DIAG("::[DBG]:{}:{} : ", __FILE__, __LINE__);                                \
  DIAG(message);                                                               \
  DIAG("\n");

#ifdef LOGLEVEL
#if LOGLEVEL == LOGV_SILENT
#define INFO(message...)
#define WARN(message...)
#define ERR(message...)
#define TRC(message...)
#define DBG(message...)
#endif
#if LOGLEVEL == LOGV_INFO
#define INFO(message...) EH_DW(EH_IFLL(LOGV_INFO_MSG(message)))
#define WARN(message...)
#define ERR(message...)
#define TRC(message...)
#define DBG(message...)
#endif
#if LOGLEVEL == LOGV_WARN
#define INFO(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_INFO, LOGV_INFO_MSG(message)))
#define WARN(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_WARN, LOGV_WARN_MSG(message)))
#define ERR(message...)
#define TRC(message...)
#define DBG(message...)
#endif
#if LOGLEVEL == LOGV_ERROR
#define INFO(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_INFO, LOGV_INFO_MSG(message)))
#define WARN(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_WARN, LOGV_WARN_MSG(message)))
#define ERR(message...)                                                        \
  EH_DW(EH_IFLL(DiagLevel::LOGV_ERROR, LOGV_ERROR_MSG(message)))
#define TRC(message...)
#define DBG(message...)
#endif
#if LOGLEVEL == LOGV_TRACE
#define INFO(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_INFO, LOGV_INFO_MSG(message)))
#define WARN(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_WARN, LOGV_WARN_MSG(message)))
#define ERR(message...)                                                        \
  EH_DW(EH_IFLL(DiagLevel::LOGV_ERROR, LOGV_ERROR_MSG(message)))
#define TRC(message...)                                                        \
  EH_DW(EH_IFLL(DiagLevel::LOGV_TRACE, LOGV_TRACE_MSG(message)))
#define DBG(message...)
#endif
#if LOGLEVEL >= LOGV_DEBUG
#define INFO(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_INFO, LOGV_INFO_MSG(message)))
#define WARN(message...)                                                       \
  EH_DW(EH_IFLL(DiagLevel::LOGV_WARN, LOGV_WARN_MSG(message)))
#define ERR(message...)                                                        \
  EH_DW(EH_IFLL(DiagLevel::LOGV_ERROR, LOGV_ERROR_MSG(message)))
#define TRC(message...)                                                        \
  EH_DW(EH_IFLL(DiagLevel::LOGV_TRACE, LOGV_TRACE_MSG(message)))
#define DBG(message...)                                                        \
  EH_DW(EH_IFLL(DiagLevel::LOGV_DEBUG, LOGV_DEBUG_MSG(message)))
#endif

#endif

#endif