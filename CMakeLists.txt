cmake_minimum_required(VERSION 3.7...3.19)

set(CMAKE_VERBOSE_MAKEFILE OFF)

project(DccExCommandline) 

#Tell Cmake where to find the sphinx module can be found ( as Sphinx  doesn't provide one )
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        7.1.3
)
FetchContent_MakeAvailable(fmt)

#FetchContent_Declare(
#  nlohmann_json
#  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
#  GIT_TAG        v3.9.1
#)
#FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
  nlohmann_json_schema_validator
  GIT_REPOSITORY https://github.com/pboettch/json-schema-validator.git
  GIT_TAG        2.1.0
)

#FetchContent_MakeAvailable(nlohmann_json_schema_validator)

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG        asio-1-18-1
)

FetchContent_MakeAvailable(asio)

find_package(fmt)
find_package(nlohmann_json)
find_package(nlohmann_json_schema_validator)

# The source code is here
add_subdirectory(src)
# docs here as well as how to build them with sphinx, breathe and exhale ( api doc )
# add_subdirectory(docs)

