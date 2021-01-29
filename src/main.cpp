#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "../include/CLI11.hpp"
#include "../include/formatter.h"

#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

#include "DccGraph.hpp"
#include "DccLayout.hpp"
#include "DccModel.hpp"
#include "DccPathFinder.hpp"
#include "DccVertex.hpp"
#include "Diag.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;


#define DCCEX_SCHEMA "../../assets/DccEXLayout.json"


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

auto main(int argc, char **argv) -> int {

  std::string dccSchemaFile = DCCEX_SCHEMA; // Default will be overirden if specified on the command line
  std::string dccLayoutFile; 

  Diag::setLogLevel(LOGV_WARN);
  Diag::setFileInfo(false);

  std::cout << "Welcome to the DCC++ EX Layout generator!\n";

  CLI::App app{"DCC++ EX Layout Generator"};

  app.get_formatter()->label("REQUIRED", "mandatory");
  app.get_formatter()->column_width(40);

  app.add_option("-s,--schema", dccSchemaFile,
                 "path/name of the DCC++ EX Layout Schema file. If this file is omitted we will\nlook for the the schema file in the current directory. If not found the system will \ncontinue but no validation of the layout will be done and this may yield \nunpredictable results")
      ->check(CLI::ExistingFile);
  app.add_option("-l,--layout", dccLayoutFile,
                 "path/name of the modelrailroad layout file")
      ->required()
      ->check(CLI::ExistingFile);
  app.add_option("-c,--commandstation", dccLayoutFile,
                 "IP address provided as e.g. 10.102.200.45 of the DCC++ EX commandstation. If the\nIP address is provided commands for creating Turnouts, Accessories etc. will be send to \nthe commandstation if we can connnect to it.\nCommandstation Version 3.0.1 is required");

  CLI11_PARSE(app, argc, argv);

  // read layout and schema
  DccLayout myLayout;

  myLayout.readLayout(dccSchemaFile, dccLayoutFile);
  myLayout.build();

  return EXIT_SUCCESS;
}
