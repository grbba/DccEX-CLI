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
 * @class DccShell
  * Interactive shell from which to interact with the tool for building, searching, connecting to the CS etc..
 * @note n/a
 * @author grbba
 */

#ifndef DccShell_h
#define DccShell_h

#define ASYNC

#include <thread>
#include <boost/asio.hpp>

#include "DccSerial.hpp"

#include "../include/cli/cli.h"



class DccShell
{
private:
    
    DccSerial serial;

    void buildMenus();
    void buildRootMenu(cli::Menu * rootMenu);
    void buildCsMenu(cli::Menu * csMenu);

public:

    auto runShell() -> int;

    DccShell() = default;
    ~DccShell() = default;
};

#endif

enum class csCmd {
    OPEN,
    READ,
    DIAG,
    CONFIG,
    RAM,
    CABS
};

struct shellCmd {
    csCmd command;
    // function pointer to the excecutor of the command
};


#define CMD_OPEN                                                               \
  "open <serial|ethernet> <port> <baud>; If serial indicate the used USB "     \
  "port and for ethernet indicate the\n\tIP address of the commandstation "   \
  "baud will be ignored for ethernet and, if not specified for serial,\n\t"  \
  "the default of 115200 will be used."