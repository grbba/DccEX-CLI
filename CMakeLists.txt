cmake_minimum_required(VERSION 3.7...3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)

project(LayoutGraph VERSION 0.1.0
                    DESCRIPTION "Generating a layout graph for operating a model railway using the DCC++ EX Commandstation"
                    LANGUAGES CXX) 

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
  # if(NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 11)
  # endif()
  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # add_compile_options(-Wall -Wextra -pedantic -Werror)
  add_compile_options(-Wall -Wextra)
  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)
  enable_testing()
  
  set(MEMORYCHECK_COMMAND FILEPATH=/usr/local/bin/valgrind)

  # Docs only available if this is the main app
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14; use our shim
if(${CMAKE_VERSION} VERSION_LESS 3.14)
  include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

# This is header only, so could be replaced with git submodules or FetchContent
find_package(Boost REQUIRED)
# Adds Boost::boost
find_package(nlohmann_json REQUIRED)
find_package(nlohmann_json_schema_validator REQUIRED)


# The compiled library code is here
add_subdirectory(src)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
