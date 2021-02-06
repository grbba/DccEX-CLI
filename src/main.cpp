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

#include <fstream>
#include <iostream>
#include <string>

// #include "../include/rang.hpp"

#include "DccConfig.hpp"
#include "DccLayout.hpp"
#include "DccShell.hpp"


/**
 * @brief Reading a file containing json;
 * Usable for small files as it fills a potentially large string.
 * For larger files direct streaming into the schema validator and/or json
 * parser should be preferred
 *
 * @param schema_filename File from which to read
 * @param schema Pointer to the string which holds the files json; filled with
 * the files content
 */
void readJsonFile(const std::string &schema_filename, std::string *schema) {
  std::ifstream schema_file(schema_filename);

  *schema = std::string(std::istreambuf_iterator<char>(schema_file),
                        std::istreambuf_iterator<char>());
}

#define HEADING(x)  rang::style::bold << rang::fg::cyan << x << rang::style::reset << rang::fg::reset

auto main(int argc, char **argv) -> int {
  // std::cout << rang::style::bold << rang::fg::cyan << "Welcome to the DCC++ EX Layout generator!\n";
  std::cout << HEADING("Welcome to the DCC++ EX Layout generator!\n\n");

  Diag::setLogLevel(DiagLevel::LOGV_INFO);
  Diag::setFileInfo(false);

  // setup the configuration
  if (!DccConfig::setup(argc, argv)) {
    // only continue if the configuration has been set properly
    return DCC_SUCCESS;
  };
  
  DccShell s;
  
  if (DccConfig::isInteractive) {
      
      // start the shell
      s.runShell();
      // on exit of the shell
      return DCC_SUCCESS;
  }

  // read layout and schema
  DccLayout myLayout;
  
  // reads the layout validates and builds the model by means of the supplied schema; 
  // creates the graph and calculates all paths through the layout (direct and indirect)
  myLayout.build();

  // get some info
  myLayout.info();

  // print out all paths
  myLayout.listPaths();

  return DCC_SUCCESS;
}
