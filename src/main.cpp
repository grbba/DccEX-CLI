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

#include <fmt/core.h>
#include <fmt/color.h>
#include <fstream>
#include <iostream>
#include <string>


#include "DccConfig.hpp"
#include "DccLayout.hpp"
#include "DccShell.hpp"

// Version information
#define MAJOR 0
#define MINOR 0
#define PATCH 10


#ifdef WIN32
#define HEADING(x)  fmt::print(x);
#define SUBHEADING(x)  fmt::print(x);
#else
#define HEADING(x)  fmt::print(fg(fmt::color::medium_turquoise) | fmt::emphasis::bold, x);
#define SUBHEADING(x)  fmt::print(fg(fmt::color::medium_turquoise), x);
#endif

auto main(int argc, char **argv) -> int {
  // clear screen
  #ifndef WIN32
    std::cout << "\e[2J\e[1;1H";
  #endif
  char month[4];
  int day, year, hour, min, sec;
  sscanf(__DATE__, "%s %i %i", &month[0], &day, &year);
  sscanf(__TIME__, "%i:%i:%i", &hour, &min, &sec);

  std::string version = fmt::format("Version {}.{}.{}", MAJOR, MINOR, PATCH);
  std::string build = fmt::format("-{}{}{}\n", day, hour, min);

#ifdef WIN32
  rang::setWinTermMode(rang::winTerm::Auto);
  std::cout << rang::fg::cyan << rang::style::bold;
#endif
  
  HEADING("Welcome to the DCC++ EX Commandline Interface\n");
  SUBHEADING(version); SUBHEADING(build);
  SUBHEADING("(c) 2020 grbba\n\n");

#ifdef WIN32
  std::cout << rang::style::reset;
#endif

  // setup the configuration including default log levels
  if (!DccConfig::setup(argc, argv)) {
    // only continue if the configuration has been set properly
    return DCC_SUCCESS;
  };

  DccShell s;
  Diag::setLogLevel(LOGV_INFO);
  
  if (DccConfig::isInteractive) {
    s.runShell();
  } else {
    // read layout and schema
    DccLayout myLayout;
    // reads the layout validates and builds the model by means of the supplied
    // schema; creates the graph and calculates all paths through the layout
    // (direct and indirect)
    
    // Diag::setFileInfo(true);
    if(!myLayout.build()) {
      return DCC_FAILURE;
    }

    
    // get some info
    myLayout.info();
    // print out all paths
    myLayout.listPaths();
  }

  return DCC_SUCCESS;
}
