cmake_minimum_required(VERSION 3.7...3.19)

project(DccExCommandline) 

find_package(fmt)
find_package(nlohmann_json_schema_validator)

# The source code is here
add_subdirectory(src)

