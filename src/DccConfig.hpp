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
 * @class DccConfig
T* his class reads the commandline option and flags and configures the run accordingly. 
 * If the -i flag is set a interactive shell will be spun up reading commands from the prompt.
 * @note n/a
 * @author grbba
 */

#ifndef DccConfig_h
#define DccConfig_h

#include <string>

#include "config.h"
#include "Diag.hpp"

#define UNOPROG arduino   // programmer for the uno
#define MEGAPROG stk500v2 // programmer for the mega
#define DCC_RELEASE "v0.0.1-alpha"
#define DCC_BUILD_REPO "https://github.com/grbba/BuildDccEX"
#define DCC_CONFIG_ZIP "DccConfig.zip"
#define DCC_CSBIN "CommandStation-EX.ino.hex"

// to be filled with DCC_BUILD_REPO, DCC_RELEASE, Architecure from the command
#define DCC_FETCH "curl -L {}/releases/download/{}/Avr_Arduino{}.zip --output {}"
// to be filled to run the avrdude command -p corresponding to the architecture
// p part i.e. m2560 for a mega atmega328p for an uno
#define DCC_AVRDUDE "{}/bin/avrdude -p {} -C {}/etc/avrdude.conf -c {} -P {} -U flash:w:{} -D &"
#define DCC_CONFIG_ROOT "./cs-config"

#ifdef __APPLE__
#define DCC_AVRDUDE_ROOT "./cs-config/avrdude/macos"
#elif
#ifdef WIN32
#define DCC_AVRDUDE_ROOT "./cs-config/avrdude/win"
#else
#define DCC_AVRDUDUE_ROOT "./cs-config/avrdude/linux"
#endif
#endif

class DccConfig
{
private:
    static std::string path;

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

    static const std::string getPath()
    {
        return path;
    };

    DccConfig() = default;
    ~DccConfig() = default;
};

#endif