cmake_minimum_required(VERSION 3.7...3.19)

set(CMAKE_VERBOSE_MAKEFILE OFF)

project(DccExCommandline) 

#Tell Cmake where to find the sphinx module can be found ( as Sphinx  doesn't provide one )
set(CMAKE_CXX_STANDARD 17)

set(CPM_DOWNLOAD_VERSION 0.31.1) 
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
        NAME fmt
        GIT_TAG 7.1.3
        GITHUB_REPOSITORY fmtlib/fmt
      )

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.7.0
    OPTIONS 
        "CMAKE_BUILD_TYPE Release"
        "SPDLOG_FMT_EXTERNAL"
    )

CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    VERSION 3.9.1)

CPMAddPackage(
    NAME nlohmann_json_validator
    GITHUB_REPOSITORY pboettch/json-schema-validator
    GIT_TAG 2.1.0
    OPTIONS
        "BUILD_TESTS OFF"
        "BUILD_EXAMPLES OFF"
    ) 



#CPMAddPackage(
#        NAME cli11
#        VERSION 1.9.1
#        GITHUB_REPOSITORY CLIUtils/CLI11
#      )

#CPMAddPackage(
#        NAME cli
#        URL https://github.com/daniele77/cli/archive/v1.2.1.zip
#        VERSION 1.2.1
        # GITHUB_REPOSITORY daniele77/cli
        #OPTIONS 
        #    "-DCLI_UseStandaloneAsio=ON"
#        )

    
find_package(Threads REQUIRED)

CPMAddPackage("gh:chriskohlhoff/asio#asio-1-18-1@1.18.1")

# ASIO doesn't use CMake, we have to configure it manually. Extra notes for using on Windows:
#
# 1) If _WIN32_WINNT is not set, ASIO assumes _WIN32_WINNT=0x0501, i.e. Windows XP target, which is
# definitely not the platform which most users target.
#
# 2) WIN32_LEAN_AND_MEAN is defined to make Winsock2 work.
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

# The source code is here
add_subdirectory(src)
# docs here as well as how to build them with sphinx, breathe and exhale ( api doc )
# add_subdirectory(docs)

