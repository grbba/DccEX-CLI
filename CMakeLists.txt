cmake_minimum_required(VERSION 3.7...3.25)

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

CPMAddPackage("gh:sphinx-doc/sphinx#v6.0.0b2")                # Document/Web generator
CPMAddPackage("gh:daniele77/cli#v2.0.2@2.0.2")                # Commandline interface library
CPMAddPackage("gh:chriskohlhoff/asio#asio-1-24-0@1.24.0")     # async communication library independnet from boost

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


find_package(fmt)
find_package(spdlog)
find_package(nlohmann_json_schema_validator)
find_package(nlohmann_json)
find_package(Threads REQUIRED)

link_directories(${DEV_HOME}/DCCEX/LayoutGraph/build/src/lib)

INCLUDE_DIRECTORIES("/usr/local/include"
                    "build/_deps/cli11-src/include"
                    "${DEV_HOME}/DCCEX/LayoutGraph/src/lib"  #includes for the LayoutGraph Library
                    ) #needed as the CPMaddpackage doesn't seem to add the includ directory properly

# The source code is here
add_subdirectory(src)
# docs here as well as how to build them with sphinx, breathe and exhale ( api doc )
add_subdirectory(docs)

