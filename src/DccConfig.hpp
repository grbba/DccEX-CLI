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
 * @class Diag
This class reads the commandline option and flags and configures the run accordingly. 
 * If the -i flag is set a interactive shell will be spun up reading commands from the prompt.
 * @note A note for doxy
 * @author grbba
 */

#ifndef DccConfig_h
#define DccConfig_h


#include <string>

#include "config.h"
#include "Diag.hpp"

class DccConfig
{
private:


public:

    /**
     * @brief reads the commandline parameters and initalizes the environment
     * 
     * @param argc  from main()
     * @param argv   from main()
     */
    static auto setup(int argc, char **argv) -> int;

    static std::string dccLayoutFile;
    static std::string dccSchemaFile;
    static bool isInteractive;
    static DiagLevel level;
    static bool fileInfo;

    DccConfig()= default;
    ~DccConfig() = default;
};



#endif