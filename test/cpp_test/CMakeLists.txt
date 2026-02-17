# Copyright 2025 Comcast Cable Communications Management, LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

if (NOT CMAKE_PROJECT_NAME)
    cmake_minimum_required(VERSION 3.10)
    project(cpp_test CXX)

    # Read SYSROOT_PATH from environment if not set as CMake variable
    if (NOT SYSROOT_PATH AND DEFINED ENV{SYSROOT_PATH})
        set(SYSROOT_PATH $ENV{SYSROOT_PATH})
        message(STATUS "Using SYSROOT_PATH from environment: ${SYSROOT_PATH}")
    endif()

    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${SYSROOT_PATH}/usr" CACHE INTERNAL "" FORCE)
    endif()

    # Add paths for finding installed packages
    if (SYSROOT_PATH)
        list(APPEND CMAKE_PREFIX_PATH 
            "${SYSROOT_PATH}/usr/lib/cmake"
            "${SYSROOT_PATH}/usr/share/cmake"
        )
        list(APPEND CMAKE_MODULE_PATH
            "${SYSROOT_PATH}/usr/lib/cmake"
            "${SYSROOT_PATH}/usr/share/cmake"
            "${SYSROOT_PATH}/tools/cmake"
        )
    endif()
    
    # Add standard system paths for finding packages
    list(APPEND CMAKE_PREFIX_PATH
        "/usr/local"
        "/usr"
    )
endif ()

set(CPP_TEST_APP cpp_test)

message("Setup ${CPP_TEST_APP}")

add_compile_options(-Wall -Wextra -Wpedantic)

# Generate header file with embedded JSON content
file(READ "../../docs/openrpc/the-spec/firebolt-open-rpc.json" JSON_CONTENT)
configure_file(
    cpp/firebolt-open-rpc.json.in
    "${CMAKE_BINARY_DIR}/generated/firebolt-open-rpc_json.h"
    @ONLY
)

file(GLOB SOURCES CONFIGURE_DEPENDS cpp/*.cpp)
message("________ ${CMAKE_SOURCE_DIR}")
add_executable(${CPP_TEST_APP}
    main.cpp
    ${SOURCES}
)

if (ENABLE_DEMO_APP)
    # Building as part of the main project - dependencies already available
    target_link_libraries(${CPP_TEST_APP}
        PRIVATE
            FireboltClient
            nlohmann_json::nlohmann_json
            nlohmann_json_schema_validator::validator
            CURL::libcurl
            Threads::Threads
    )
    set(SOURCE_ROOT ${CMAKE_SOURCE_DIR})
else()
    # Building standalone - find all installed dependencies
    find_package(FireboltTransport CONFIG REQUIRED)
    find_package(FireboltClient CONFIG REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(nlohmann_json_schema_validator REQUIRED)
    find_package(CURL REQUIRED)
    find_package(Threads REQUIRED)
    target_link_libraries(${CPP_TEST_APP}
        PRIVATE
            FireboltTransport::FireboltTransport
            FireboltClient::FireboltClient
            nlohmann_json::nlohmann_json
            nlohmann_json_schema_validator::validator
            CURL::libcurl
            Threads::Threads
    )
       set(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/../..)
endif ()

message("________ ${SOURCE_ROOT}")

target_include_directories(${CPP_TEST_APP}
    PRIVATE

        $<BUILD_INTERFACE:${SOURCE_ROOT}/include/>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/>
        $<BUILD_INTERFACE:${SOURCE_ROOT}/test/>
        $<BUILD_INTERFACE:${SOURCE_ROOT}/src/>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
)

add_compile_definitions(UT_OPEN_RPC_FILE="firebolt-open-rpc.json")

set_target_properties(${CPP_TEST_APP} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

install(
    TARGETS ${CPP_TEST_APP}
    DESTINATION bin
)
