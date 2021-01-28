#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <chrono>

#include "../include/formatter.h"
#include <boost/algorithm/string/trim.hpp>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include "Diag.hpp"
#include "DccModel.hpp"
#include "DccVertex.hpp"
#include "DccGraph.hpp"
#include "DccPathFinder.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;


std::string const fLayout = "../../assets/TimeSaver2Modules.json";
// std::string const fLayout = "../../assets/TimeSaver.json";
// std::string const fLayout = "../../assets/SuperSmall.json";
// std::string const fLayout = "../../assets/SimpleTest.json";
std::string const dccSchema = "../../assets/DccEXLayout.json";

/**
 * @brief Reading a file containing json; 
 * Usable for small files as it fills a potentially large string. 
 * For larger files direct streaming into the schema validator and/or json parser should be preferred
 * 
 * @param file File from which to read
 * @param json Pointer to the string which holds the files json; filled with the files content
 * @return true  reading was successful
 * @return false reading failed
 */

void readJsonFile(const std::string &schema_filename, std::string *schema)
{
	std::ifstream schema_file(schema_filename);

	*schema = std::string(std::istreambuf_iterator<char>(schema_file),
	                      std::istreambuf_iterator<char>());
}

int buildLayout(dccexlayout::DccExLayout *layout)
{
    INFO("Building Layout ... : %s\n", layout->get_layout().get_name());
    INFO("%s has %d module(s)", layout->get_layout().get_name(), layout->get_modules().size());

    DccGraph g;            // g will hold the full graph with all vertices ( with their connections i.e. is build in adjacency list form / double vertices etc ...
    DccPathFinder gpf(&g); // setup the Pathfinder for the graph g -> to be inialized after the graph has been build 

    auto start = std::chrono::high_resolution_clock::now();

    // feed the layout into the graph builder
    g.build(layout);

    for (auto var : *g.getDvGraph())
    {
        var.second.get()->printDoubleVertex();
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    // g.printTrackElements();
    // get some info about the build layout/graph
    
    g.printInfo();
    gpf.init();
    gpf.printAllPaths();

    // gpf.findAllPaths(1, 20);
    // gpf.findAllPaths(DccVertex::cantorEncode(1,9));

    // INFO("Finished computation at %s", std::ctime(&end_time));
    INFO("Elapsed time: [%d]ms", duration.count());

    return 0;
}

int main()
{
    std::string dccExSchema;
    std::ifstream layoutFile(fLayout);
    std::ifstream schemaFile(dccSchema);
    dccexlayout::DccExLayout layout;
    json_validator validator; // (nullptr, nlohmann::json_schema::default_string_format_check); // create validator

    Diag::setLogLevel(LOGV_WARN);
    Diag::setFileInfo(false);

    json schema;

    INFO("Welcome to the DCC++ EX Layout generator!\n");
    INFO("Reading & validating DccExLayout schema ...");

    try
    {
        schemaFile >> schema;
    }
    catch (const std::exception &e)
    {
        ERR("Validation of schema failed: %s", e.what());
        ERR("Validation failed at: [%s] while parsing schema", schemaFile.tellg());
        return EXIT_FAILURE;
    }

    INFO("Initalizing DccExLayout schema for layout parsing ...");
    try
    {
        validator.set_root_schema(schema); // insert root-schema
    }
    catch (const std::exception &e)
    {
        ERR("Schema initalization failed: %s", e.what());
        return EXIT_FAILURE;
    }

    INFO("Validating layout description ... ");
    json document;
    try
    {
        layoutFile >> document;
    }
    catch (const std::exception &e)
    {
        
        ERR("Validation failed at: [%s] while parsing layout", schemaFile.tellg());
        return EXIT_FAILURE;
    }

    try
    {

        validator.validate(document);
    }
    catch (const std::exception &e)
    {
        ERR("Validation of parsed layout failed: %s", e.what());
        return EXIT_FAILURE;
    }

    INFO("Loading layout description ... ");

    // reset to start position of the file as it was read before for the Validation step

    layoutFile.clear();
    layoutFile.seekg(0);

    try
    {
        layout = nlohmann::json::parse(layoutFile); 
    }
    catch (json::parse_error &e)
    {
        ERR("Loading of the layout failed: %s", e.what());
        return EXIT_FAILURE;
    }

    buildLayout(&layout);

    return EXIT_SUCCESS;
}
