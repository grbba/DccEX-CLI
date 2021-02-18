
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

#include <iostream>
#include <vector>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

#include "DccSerial.hpp"

#define HEADING(x)  fmt::format(fg(fmt::color::medium_turquoise) | fmt::emphasis::bold, x);
#define WARNING(x)  fmt::format(fg(fmt::color::orange) | fmt::emphasis::bold, x);
#define ERROR(x)  fmt::format(fg(fmt::color::red) | fmt::emphasis::bold, x);

// std::ostream &DccSerial::out;
/**
 * @brief
 *
 * @param data
 * @param len
 */
void DccSerial::recieve(const char *data, unsigned int len) {
  std::vector<char> v(data, data + len);
  // std::cout << rang::fg::magenta;
  for (unsigned int i = 0; i < v.size(); i++) {
    if (v[i] == '\n') {
      std::cout << '\n';
    } else {
      if (v[i] < 32 || v[i] >= 0x7f)
        std::cout.put(' '); // Remove non-ascii char
      else
      fmt::print(fg(fmt::color::magenta),"{}", v[i]);
      // std::cout.put(v[i]);
    }
  }
  //std::cout << rang::fg::reset;
  std::cout.flush(); // Flush screen buffer
}

/**
 * @brief
 *
 * @param out
 * @param type
 * @param device
 * @param baud
 * @return true
 * @return false
 */
bool DccSerial::openPort(std::ostream &out, std::string type, std::string d, int b) {
  device = d;
  int8_t cType = -1;
  if (type.compare("serial") == 0) {
    cType = DCC_SERIAL;
  } else {
    if (type.compare("ethernet") == 0) {
      out << ERROR("Ethernet is not yet supported.\n");
      cType = DCC_ETHERNET;
      return DCC_FAILURE;
    } else {
      cType = DCC_CONN_UNKOWN;
      out << ERROR("Unknown connection type specified.\n");
      return DCC_FAILURE;
    }
  }

  switch(cType) {
  case DCC_SERIAL: {
    baud = b;
    port.setCallback(recieve);
    port.open(device, baud);
    if (port.isOpen()) {
      open = true;
    } else {
           out << ERROR("Could not open serial port. Maybe in use by another program ?\n");
    }
    break;
  }
  case DCC_ETHERNET: {
    // connect over Ethernet
    break;
  }
  case DCC_CONN_UNKOWN: {
    // error wrong command
    break;
  }
  }
  return DCC_SUCCESS;
}

/**
 * @brief
 *
 */
bool DccSerial::openPort() {
  port.setCallback(recieve);
  port.open(device, baud);
  return port.isOpen();
};

/**
 * @brief
 *
 */
void DccSerial::closePort() {
  port.clearCallback();
  port.close();
};

/**
 * @brief
 *
 */
void DccSerial::write() {
  char s[3] = {0x04, '\n'};
  port.write(s, 3);
};

void DccSerial::write(const std::string *cmd) {
  port.write(cmd->c_str(), cmd->size());
};

// void DccSerial::execute(shellCommand cmd, std::ostream &out, CmdParam_t p1,CmdParam_t p2,CmdParam_t p3) {
//   switch(cmd) {
//     case shellCommand::S_OPEN:{
//       // this->openPort(out, p1,p2,p3);
//       break;
//     }
//     case shellCommand::S_STATUS:{
//       break;
//     }
//     default: {
//       out << "Unspecified command";
//     }
//   }
// }
