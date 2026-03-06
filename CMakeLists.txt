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

cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(FireboltClient)

include(version)

message(STATUS "Project version: ${PROJECT_VERSION}")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_TESTS "Build tests" OFF)
option(DISCOVER_UT "Discover all Unit Tests" ON)
option(DISCOVER_CT "Discover all Component Tests" OFF)
option(ENABLE_DEMO_APP "Build demo app" OFF)
option(BUILD_WITH_INSTALLED_TRANSPORT "Build the library with the transport that is installed, even if its version mismatches" ON)

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

find_package(nlohmann_json CONFIG REQUIRED)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/.transport.version" FIREBOLT_TRANSPORT_VERSION_RAW)
string(STRIP "${FIREBOLT_TRANSPORT_VERSION_RAW}" FIREBOLT_TRANSPORT_VERSION)
string(REGEX REPLACE "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\1" FIREBOLT_TRANSPORT_MAJOR_VERSION "${FIREBOLT_TRANSPORT_VERSION}")

find_package(FireboltTransport ${FIREBOLT_TRANSPORT_MAJOR_VERSION} CONFIG REQUIRED)

message(STATUS "Transport: installed version: ${FireboltTransport_VERSION}, expected: ${FIREBOLT_TRANSPORT_VERSION}")

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)

if (ENABLE_DEMO_APP)
    add_subdirectory(test/api_test_app)
endif()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif()
