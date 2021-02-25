cmake_minimum_required(VERSION 3.7...3.19)

project(DccExCommandline) 

#Tell Cmake where to find the sphinx module can be found ( as Sphinx  doesn't provide one )
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


find_package(fmt)
find_package(nlohmann_json_schema_validator)

# The source code is here
add_subdirectory(src)
add_subdirectory(docs)

