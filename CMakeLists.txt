cmake_minimum_required(VERSION 3.12 )
set(CMAKE_MACOSX_RPATH 1)


set(DEV_HOME "/Users/gregor/Development") #home directory for all development stuff

set(CMAKE_VERBOSE_MAKEFILE OFF)

project(DccExCommandline) 

set(CMAKE_CXX_STANDARD 17)
set(CPM_DOWNLOAD_VERSION 0.36.0) 
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage("gh:CLIUtils/CLI11#v2.3.1@2.3.1")             # tools for reading the command line not to be mixed with 
                                                            # cli which manages cli menus
if(CLI11_ADDED)
    target_include_directories(CLI11 SYSTEM INTERFACE ${CLI11_SOURCE_DIR}/include/CLI)
endif()

CPMAddPackage( NAME fmt
        GIT_TAG 9.1.0
        GITHUB_REPOSITORY fmtlib/fmt
      )
      
CPMAddPackage( NAME spdlog
        "gh:gabime/spdlog@1.11.0"
        OPTIONS 
            "CMAKE_BUILD_TYPE Release"
            "SPDLOG_FMT_EXTERNAL"
    )

CPMAddPackage( NAME nlohmann_json
        VERSION 3.11.2
        GITHUB_REPOSITORY nlohmann/json
        OPTIONS
          "JSON_BuildTests OFF"
          "JSON_Install ON"
    )

CPMAddPackage( NAME nlohmann_json_validator
    GITHUB_REPOSITORY pboettch/json-schema-validator
    GIT_TAG 2.2.0
    OPTIONS
        "BUILD_TESTS OFF"
        "BUILD_EXAMPLES OFF"
    ) 

CPMAddPackage( NAME paho_mqtt_c 
    GITHUB_REPOSITORY eclipse/paho.mqtt.c
    GIT_TAG v1.3.12
    OPTIONS 
        "PAHO_ENABLE_TESTING OFF"
        "PAHO_BUILD_STATIC ON"
        "PAHO_HIGH_PERFORMANCE ON"
    )

CPMAddPackage( NAME sphinx
    "gh:sphinx-doc/sphinx#v6.0.0b2")                # Document/Web generator
CPMAddPackage( NAME cli 
    "gh:daniele77/cli#v2.0.2@2.0.2")                # Commandline interface library
CPMAddPackage( NAME asio 
    "gh:chriskohlhoff/asio#asio-1-24-0@1.24.0")     # async communication library independnet from boost

if(asio_ADDED)
    add_library(asio INTERFACE)
    target_include_directories(asio SYSTEM INTERFACE ${asio_SOURCE_DIR}/asio/include)
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
    target_link_libraries(asio INTERFACE Threads::Threads)

    if(WIN32)
    # macro see @ https://stackoverflow.com/a/40217291/1746503
    macro(get_win32_winnt version)
        if(CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
        string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
        # Check for Windows 10, b/c we'll need to convert to hex 'A'.
        if("${verMajor}" MATCHES "10")
            set(verMajor "A")
            string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
        endif("${verMajor}" MATCHES "10")
        # Remove all remaining '.' characters.
        string(REPLACE "." "" ver ${ver})
        # Prepend each digit with a zero.
        string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
        endif()
    endmacro()

    if(NOT DEFINED _WIN32_WINNT)
        get_win32_winnt(ver)
        set(_WIN32_WINNT ${ver})
    endif()

    message(STATUS "Set _WIN32_WINNET=${_WIN32_WINNT}")

    target_compile_definitions(asio INTERFACE _WIN32_WINNT=${_WIN32_WINNT} WIN32_LEAN_AND_MEAN)
    endif()
endif()

#find_package(fmt)
#find_package(spdlog)
# find_package(nlohmann_json_schema_validator)
#find_package(nlohmann_json)

find_package(Threads REQUIRED)

link_directories(${DEV_HOME}/DCCEX/LayoutGraph/build/src/lib)

INCLUDE_DIRECTORIES("/usr/local/include"
                    "build/_deps/cli11-src/include"
                    "${DEV_HOME}/DCCEX/LayoutGraph/src/lib"  #includes for the LayoutGraph Library
                    ) #needed as the CPMaddpackage doesn't seem to add the includ directory properly

#add the MQTT requirements
# find_package(OpenSSL REQUIRED)
find_library(paho-mqttpp3 NAMES libpaho-mqttpp3.a REQUIRED)
add_library(pahottpp STATIC IMPORTED)
set_target_properties(pahottpp PROPERTIES IMPORTED_LOCATION ${paho-mqttpp3})

# The source code is here
add_subdirectory(src)
# docs here as well as how to build them with sphinx, breathe and exhale ( api doc )
# add_subdirectory(docs)

