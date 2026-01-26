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

cmake_minimum_required(VERSION 3.10)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(FireboltClient)

include(version)

if(EXISTS "${CMAKE_SOURCE_DIR}/.transport.version")
    file(READ "${CMAKE_SOURCE_DIR}/.transport.version" FIREBOLT_TRANSPORT_VERSION)
    string(STRIP ${FIREBOLT_TRANSPORT_VERSION} FIREBOLT_TRANSPORT_VERSION)
else()
    message(FATAL_ERROR "Could not determine firebolt-cpp-transport version from .transport.version")
endif()

message(STATUS "Project version: ${PROJECT_VERSION}")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_TESTS "Build tests" OFF)
option(DISCOVER_UT "Discover all Unit Tests" ON)
option(DISCOVER_CT "Discover all Component Tests" OFF)
option(ENABLE_DEMO_APP "Build demo app" OFF)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${SYSROOT_PATH}/usr" CACHE INTERNAL "" FORCE)
    set(CMAKE_PREFIX_PATH ${SYSROOT_PATH}/usr/lib/cmake CACHE INTERNAL "" FORCE)
endif()

list(APPEND CMAKE_PREFIX_PATH
    "${SYSROOT_PATH}/usr/share/cmake")

list(APPEND CMAKE_MODULE_PATH
    "${SYSROOT_PATH}/usr/share/cmake"
    "${SYSROOT_PATH}/usr/lib/cmake"
)

if(NOT TARGET nlohmann_json::nlohmann_json)
    find_package(nlohmann_json REQUIRED)
endif()

if(NOT TARGET websocketpp::websocketpp)
    find_package(websocketpp REQUIRED)
endif()

find_package(FireboltTransport CONFIG QUIET)

if(FireboltTransport_FOUND)
    check_version_compatibility(FireboltTransport_VERSION_OK "${FireboltTransport_VERSION}" "${FIREBOLT_TRANSPORT_VERSION}")
else()
    set(FireboltTransport_VERSION_OK FALSE)
endif()

if(FireboltTransport_VERSION_OK)
    message(STATUS "Installed FireboltTransport, ${FireboltTransport_VERSION}, matches expected version: ${FIREBOLT_TRANSPORT_VERSION}")
else()
    message(STATUS "FireboltTransport version mismatch or not found. Will try to fetch it."
                   " Expected '${FIREBOLT_TRANSPORT_VERSION}' but found '${FireboltTransport_VERSION}'")
    FetchContent_Declare(
        FireboltTransport
        URL "https://github.com/rdkcentral/firebolt-cpp-transport/releases/download/v${FIREBOLT_TRANSPORT_VERSION}/firebolt-cpp-transport-${FIREBOLT_TRANSPORT_VERSION}.tar.gz"
        URL_HASH "SHA256=2ff666c266ec22f9ed7989b1c4d3a7c6c2df24a8880d8587179236ccfac24163"
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    set(ENABLE_TESTS_ORIG ${ENABLE_TESTS})
    set(ENABLE_TESTS OFF)
    FetchContent_MakeAvailable(FireboltTransport)
    set(ENABLE_TESTS ${ENABLE_TESTS_ORIG})
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)

if (ENABLE_DEMO_APP)
    add_subdirectory(demo)
    add_subdirectory(test/cpp_test) #for now
endif()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif()

