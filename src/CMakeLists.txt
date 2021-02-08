cmake_minimum_required(VERSION 3.7...3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(CTest)
enable_testing()

include_directories(../include ../../boost_1_75_0 /usr/local/Cellar/nlohmann-json/3.9.1_1/include /usr/local/Cellar/llvm/11.0.0_1/include )

add_executable( dcclayout 
                main.cpp 
                Diag.cpp 
                DccVertex.cpp
                DccGraph.cpp 
                DccPathFinder.cpp
                DccLayout.cpp
                DccConfig.cpp
                DccPath.cpp
                DccShell.cpp
                AsyncSerial.cpp
                BufferedAsyncSerial.cpp
              )

target_compile_features(dcclayout PRIVATE cxx_std_17)
target_link_libraries(dcclayout nlohmann_json_schema_validator)

find_program(CLANG_TIDY_BIN clang-tidy)
find_program(RUN_CLANG_TIDY_BIN /usr/local/bin/run-clang-tidy.py)
  list(APPEND RUN_CLANG_TIDY_BIN_ARGS -clang-tidy-binary ${CLANG_TIDY_BIN} 
  -header-filter = .* -checks=clan*,cert*,misc*,perf*,cppc*,mode*,-cert-err58-cpp,-misc-noexcept-move-constructor,-cppcoreguidelines-*)
  add_custom_target(tidy COMMAND ${RUN_CLANG_TIDY_BIN} ${RUN_CLANG_TIDY_BIN_ARGS} COMMENT "running clang tidy")