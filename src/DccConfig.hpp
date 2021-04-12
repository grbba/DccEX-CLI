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

#include <fmt/core.h>
#include "Diag.hpp"
#include "DccSerial.hpp"
#include "DccTCP.hpp"

#if defined(__unix__) || defined(__unix) || defined(__linux__)
#define OS_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define OS_WIN
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MAC
#else
#error Unknown Platform
#endif

#define CONFIG_INTERACTIVE false
#define CONFIG_FILEINFO false

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

#define DCC_CONFIG_ROOT "./cs-config" // all config elated stuff avrdude, cs binaries etc go here
#define DCC_ASSETS_ROOT "./cs-assets" // schemas, layouts etc..

#define CONFIG_DCCEX_SCHEMA "./cs-assets/DccEXLayout.json"
#define DCC_DEFAULT_BAUDRATE 115200
#define DCC_DEFAULT_PORT 2560

#define DCC_FAILURE 0
#define DCC_SUCCESS 1

// #define DCC_SERIAL 0
// #define DCC_ETHERNET 1
// #define DCC_CONN_UNKOWN 2

#ifdef OS_MAC
#define DCC_AVRDUDE_ROOT "./cs-config/avrdude/macos"
#endif
#ifdef OS_WIN
#define DCC_AVRDUDE_ROOT "./cs-config/avrdude/win"
#endif
#ifdef OS_LINUX
#define DCC_AVRDUDE_ROOT "./cs-config/avrdude/linux"
#endif

enum CsConnection_t
{
    DCC_SERIAL,
    DCC_ETHERNET,
    DCC_CONN_UNKOWN
};

// Arduino defines ...

// from MotorDrivers.h in the CS code

// MotorDriver(byte power_pin, byte signal_pin, byte signal_pin2, int8_t brake_pin, byte current_pin,
//             float senseFactor, unsigned int tripMilliamps, byte faultPin);

#ifndef UNUSED_PIN     // sync define with the one in MotorDriver.h
#define UNUSED_PIN 127 // inside int8_t
#endif

// Available standard MotorShields
enum CsMotorShield
{
    STANDARD_MOTOR_SHIELD,
    POLOLU_MOTOR_SHIELD,
    FIREBOX_MK1,
    FIREBOX_MK1S,
    FUNDUMOTO_SHIELD,
    IBT_2_WITH_ARDUINO,
    NOT_CONFIGURED
};

// used in the Session code of the cs in order to map the pin
// to the pin of the corresponding architecture
// e.g; A5 on mega may be different to A5 on an uno
// so when we send the MotorDriver structure before creating the
// Motordriver in the CS we need to map those pins properly

enum CsPin
{
    A0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A23 = 23,  
    A29 = 29
};

enum CsOut
{
    MAIN,
    PROG
};

struct MotorDriver
{
    CsMotorShield id;
    CsOut track;
    uint8_t power_pin;
    uint8_t signal_pin;
    uint8_t signal_pin2;
    int8_t brake_pin;
    CsPin current_pin;
    float senseFactor;
    unsigned int tripMilliamps;
    uint8_t faultPin;
    
    std::string toString() {
        auto s = fmt::format("[{}:{}]{}|{}|{}|{}|{}|{}|{}|{}|", id, track, power_pin, signal_pin, signal_pin2, brake_pin, current_pin, senseFactor, tripMilliamps, faultPin );
        return s;
    }
};

struct MotorShield {
    std::string name;
    MotorDriver main;
    MotorDriver prog;
    
    std::string toString() {
        auto s = fmt::format("%{}:{}:{}%", name, main.toString(), prog.toString());
        return s;
    }
};

// MotorDriver definitions

// Arduino standard Motor Shield
// #define STANDARD_MOTOR_SHIELD F("STANDARD_MOTOR_SHIELD"),  
const MotorDriver mdStdMain({STANDARD_MOTOR_SHIELD, MAIN, 3, 6, 7, UNUSED_PIN, A5, 9.766, 5500, UNUSED_PIN});           // main
const MotorDriver mdStdProg({STANDARD_MOTOR_SHIELD, PROG, 11, 13, UNUSED_PIN, UNUSED_PIN, A1, 2.99, 2000, UNUSED_PIN}); // prog
const MotorShield standard({"Standard Arduino", mdStdMain, mdStdProg}); 

// Pololu Motor Shield
// #define POLOLU_MOTOR_SHIELD F("POLOLU_MOTOR_SHIELD"),  
const MotorDriver mdPololuMain({POLOLU_MOTOR_SHIELD, MAIN, 9, 7, UNUSED_PIN,         -4, A0, 18, 3000, 12});
const MotorDriver mdPololuProg({POLOLU_MOTOR_SHIELD, PROG, 10, 8, UNUSED_PIN, UNUSED_PIN, A1, 18, 3000, 12});
const MotorShield pololu({"Pololu", mdPololuMain, mdPololuProg}); 

// #define FIREBOX_MK1 F("FIREBOX_MK1"),                                                  
const MotorDriver mdFBMK1Main({FIREBOX_MK1, MAIN, 3, 6, 7, UNUSED_PIN, A5, 9.766, 5500, UNUSED_PIN});
const MotorDriver mdFBMK1Prog({FIREBOX_MK1, PROG, 4, 8, 9, UNUSED_PIN, A1, 5.00, 1000, UNUSED_PIN});
const MotorShield fbmk1({"Firebox MK1", mdFBMK1Main, mdFBMK1Prog}); 

// Firebox Mk1S
// #define FIREBOX_MK1S F("FIREBOX_MK1A"),                                            
const MotorDriver mdFBMK1SMain({FIREBOX_MK1S, MAIN, 24, 21, 22, 25, A23, 9.766, 5500, UNUSED_PIN});
const MotorDriver mdFBMK1SProg({FIREBOX_MK1S, PROG, 30, 27, 28, 31, A29, 5.00, 1000, UNUSED_PIN});
const MotorShield fbmk1s({"Firebox MK1S", mdFBMK1SMain, mdFBMK1SProg}); 

// FunduMoto Motor Shield
// #define FUNDUMOTO_SHIELD F("FUNDUMOTO_SHIELD"),                                              
const MotorDriver mdFUNDUMain({FUNDUMOTO_SHIELD, MAIN, 10, 12, UNUSED_PIN, 9, A0, 2.99, 2000, UNUSED_PIN}); 
const MotorDriver mdFUNDUProg({FUNDUMOTO_SHIELD, PROG, 11, 13, UNUSED_PIN, UNUSED_PIN, A1, 2.99, 2000, UNUSED_PIN});
const MotorShield fundumoto({"Fundumoto", mdFUNDUMain, mdFUNDUProg}); 

// IBT_2 Motor Board for Main and Arduino Motor Shield for Prog
// #define IBT_2_WITH_ARDUINO F("IBT_2_WITH_ARDUINO_SHIELD"),                                              
const MotorDriver mdIBTMain({IBT_2_WITH_ARDUINO, MAIN, 4, 5, 6, UNUSED_PIN, A5, 41.54, 5000, UNUSED_PIN});
const MotorDriver mdIBTProg({IBT_2_WITH_ARDUINO, PROG, 11, 13, UNUSED_PIN, UNUSED_PIN, A1, 2.99, 2000, UNUSED_PIN});
const MotorShield ibt2({"IBT 2", mdIBTMain, mdIBTProg}); 

const std::map<CsMotorShield, MotorShield> MotorShields { 
    { STANDARD_MOTOR_SHIELD, standard }, 
    { POLOLU_MOTOR_SHIELD, pololu },
    { FIREBOX_MK1, fbmk1 },
    { FIREBOX_MK1S, fbmk1s },
    { FUNDUMOTO_SHIELD, fundumoto },
    { IBT_2_WITH_ARDUINO, ibt2 }
    };


// typedef std::variant< DccSerial,
//                       DccTCP > CsConnection_t;

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

    static std::string  dccLayoutFile;
    static std::string  dccSchemaFile;
    static std::string  mcu;
    static std::string  port;
    static bool         isInteractive;      // run as interactive shell
    static bool         isUpload;           // Upload has been requested
    static bool         isConnect;          // Connection to the cs has been requested from the commandline
    static DiagLevel    level;
    static bool         fileInfo;
    static int          baud;               // baud rate for the serial connection; if not set then default is 115200
    static DccSerial    serial;             // Serial port instance
    static DccTCP       ethernet;           // Ethernet connection ( we only allow one ...)
    static CsConnection_t active;           // Active connection type ( maps either to serial or ethernet)
    static CsMotorShield  mshield;          // Mototshield configure init with NOT_CONFIGURED
    static bool          setMshield;        // set by the mshield command to get through the smencmd mototshield available check

    static const std::string getPath()
    {
        return path;
    };

    DccConfig() = default;
    ~DccConfig() = default;
};

#endif