cmake_minimum_required(VERSION 3.10)
project(cpp_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp_test 
    main.cpp
    cpp/advertisingDemo.cpp
    cpp/accessibilityDemo.cpp
    cpp/chooseInterface.cpp
    cpp/deviceDemo.cpp
    cpp/displayDemo.cpp
    cpp/fireboltdemo.cpp
    cpp/lifecycleDemo.cpp
    cpp/localizationDemo.cpp
    cpp/presentationDemo.cpp
    cpp/statsDemo.cpp
    cpp/utils.cpp
)

list(APPEND CMAKE_PREFIX_PATH ${SYSROOT_PATH}) 

find_package(FireboltTransport)
find_package(nlohmann_json REQUIRED)
find_package(nlohmann_json_schema_validator REQUIRED)
find_package(CURL REQUIRED)

# Generate header file with embedded JSON content
file(READ "../../docs/openrpc/the-spec/firebolt-open-rpc.json" JSON_CONTENT)
configure_file(
    cpp/firebolt-open-rpc.json.in
    "${CMAKE_BINARY_DIR}/generated/firebolt-open-rpc_json.h"
    @ONLY
)

# Make sure your target can find the generated header
target_include_directories(cpp_test PRIVATE ${CMAKE_BINARY_DIR}/generated)

target_include_directories(cpp_test PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/test/>
)

add_compile_definitions(UT_OPEN_RPC_FILE="firebolt-open-rpc.json")

# Find Threads package for pthread
find_package(Threads REQUIRED)

target_link_libraries(cpp_test
    FireboltClient
    FireboltTransport::FireboltTransport
    nlohmann_json::nlohmann_json
    nlohmann_json_schema_validator::validator
    CURL::libcurl
    Threads::Threads
)