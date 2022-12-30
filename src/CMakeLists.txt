cmake_minimum_required(VERSION 3.12...3.25)

set(DEV_HOME "/Users/gregor/Development") #home directory for all development stuff
include_directories( /usr/local/include )

add_executable( dcccli
                main.cpp 
                Diag.cpp 
                DccConfig.cpp
                DccShell.cpp
                AsyncSerial.cpp
                DccSerial.cpp
                AsyncTCP.cpp 
                DccTCP.cpp
                DccMQTT.cpp 
                DccShellCmd.cpp
                ShellCmdExec.cpp
              )

# target_compile_features(dcclayout PRIVATE cxx_std_17)
# target_link_libraries(dcclayout nlohmann_json_schema_validator nlohmann_json::nlohmann_json fmt::fmt)

target_link_libraries(dcccli 
                      libDccLayout # Railnetwork layout management library ( from the LayoutGraph project )
                      asio 
                      fmt::fmt 
                      spdlog::spdlog 
                      nlohmann_json::nlohmann_json 
                      nlohmann_json_schema_validator 
                      cli::cli
                      paho-mqtt3c 
                      paho-mqtt3a 
                      pahottpp)

target_compile_options(dcccli PRIVATE -Wno-deprecated-declarations)

install(TARGETS dcccli DESTINATION bin)
#find_program(CLANG_TIDY_BIN clang-tidy)
#find_program(RUN_CLANG_TIDY_BIN /usr/local/bin/run-clang-tidy.py)
#  list(APPEND RUN_CLANG_TIDY_BIN_ARGS -clang-tidy-binary ${CLANG_TIDY_BIN} 
#  -header-filter = .* -checks=clan*,cert*,misc*,perf*,cppc*,mode*,-cert-err58-cpp,-misc-noexcept-move-constructor,-cppcoreguidelines-*)
#  add_custom_target(tidy COMMAND ${RUN_CLANG_TIDY_BIN} ${RUN_CLANG_TIDY_BIN_ARGS} COMMENT "running clang tidy")