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

#include <thread>

#include <boost/asio.hpp>

#include "../include/cli/cli.h"
#include "../include/cli/clifilesession.h"
#include <fstream>

using namespace cli;

class DccShell
{
private:

    bool done = false;
    Cli *_pCli = nullptr;
    CliFileSession *_pInput = nullptr;
    std::thread reciever;

public:

    // static bool shellExit; 
    auto runShell() -> int;
    bool isDone() {
        return done;
    }

    DccShell() = default;
    ~DccShell() = default;
};

#endif