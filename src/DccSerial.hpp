
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

#ifndef DccSerial_h
#define DccSerial_h

#include "AsyncSerial.h"
#include <variant>

#define DCC_FAILURE 0
#define DCC_SUCCESS 1

#define DCC_SERIAL 0
#define DCC_ETHERNET 1
#define DCC_CONN_UNKOWN 2

// goes into DccShell.hpp
enum class shellCommand {
  S_OPEN,
  S_STATUS
};

typedef std::variant< std::ostream,
                      std::string,
                      int> CmdParam_t;

class DccSerial {
private:
  CallbackAsyncSerial port;
  int baud = 115200;                                // default is 115200
  std::string device;
  bool open = false;
  // std::ostream *out;
  // CmdParam_t cmd;  

  static void recieve(const char *data, unsigned int len); // callback for reading


public:
  bool openPort();  // open the port
  bool openPort(std::ostream &out, std::string type, std::string device, int baud );  // open the port
  void closePort(); // close the port
  void write();     // write to the port
  void write(const std::string *cmd);

  void setDevice(std::string d) { device = d; }
  void setBaud(int b) { baud = b; }

  bool isOpen() { return open; };
  // void setCmd(CmdParam_t c) { cmd = c; }            // current command to process
  // void execute(shellCommand cmd, std::ostream &out, CmdParam_t p1,CmdParam_t p2,CmdParam_t p3);

  DccSerial() = default;
  ~DccSerial() = default;
  DccSerial(std::string d, int b) : baud(b), device(d) {};
};

#endif


