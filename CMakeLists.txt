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
option(DISABLE_SO_VERSION "Disable SONAME/SOVERSION of shared library" OFF)

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

set(FIREBOLT_TRANSPORT_ROOT "" CACHE PATH
    "Optional install prefix for FireboltTransport (contains lib/cmake/FireboltTransport)"
)

if(NOT FIREBOLT_TRANSPORT_ROOT AND DEFINED ENV{FIREBOLT_TRANSPORT_ROOT} AND NOT "$ENV{FIREBOLT_TRANSPORT_ROOT}" STREQUAL "")
    set(FIREBOLT_TRANSPORT_ROOT "$ENV{FIREBOLT_TRANSPORT_ROOT}" CACHE PATH
        "Optional install prefix for FireboltTransport (contains lib/cmake/FireboltTransport)" FORCE)
endif()

if(FIREBOLT_TRANSPORT_ROOT)
    unset(FireboltTransport_DIR CACHE)
    list(INSERT CMAKE_PREFIX_PATH 0 "${FIREBOLT_TRANSPORT_ROOT}")
endif()

if(NOT FireboltTransport_ROOT AND DEFINED ENV{FireboltTransport_ROOT})
    set(FireboltTransport_ROOT "$ENV{FireboltTransport_ROOT}")
endif()

if(NOT FireboltTransport_ROOT AND FIREBOLT_TRANSPORT_ROOT)
    set(FireboltTransport_ROOT "${FIREBOLT_TRANSPORT_ROOT}")
endif()

set(BUILD_WITH_INSTALLED_TRANSPORT_DEFAULT OFF)
if(FIREBOLT_TRANSPORT_ROOT OR FireboltTransport_ROOT)
    set(BUILD_WITH_INSTALLED_TRANSPORT_DEFAULT ON)
endif()
option(BUILD_WITH_INSTALLED_TRANSPORT
    "Allow a version mismatch when an explicit transport root is supplied"
    ${BUILD_WITH_INSTALLED_TRANSPORT_DEFAULT}
)

find_package(nlohmann_json CONFIG REQUIRED)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/.transport.version"
)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/.transport.version" FIREBOLT_TRANSPORT_VERSION_RAW)
string(STRIP "${FIREBOLT_TRANSPORT_VERSION_RAW}" FIREBOLT_TRANSPORT_VERSION)

if(BUILD_WITH_INSTALLED_TRANSPORT AND (FIREBOLT_TRANSPORT_ROOT OR FireboltTransport_ROOT))
    find_package(FireboltTransport CONFIG REQUIRED)
    if(FireboltTransport_VERSION VERSION_LESS FIREBOLT_TRANSPORT_VERSION)
        message(WARNING "Using FireboltTransport ${FireboltTransport_VERSION} although ${FIREBOLT_TRANSPORT_VERSION} is required; development version mismatch is enabled")
    else()
        message(STATUS "Transport: installed version: ${FireboltTransport_VERSION}, minimum required: ${FIREBOLT_TRANSPORT_VERSION}")
    endif()
else()
    if(BUILD_WITH_INSTALLED_TRANSPORT)
        message(STATUS "Ignoring BUILD_WITH_INSTALLED_TRANSPORT without an explicit FireboltTransport root")
    endif()
    find_package(FireboltTransport ${FIREBOLT_TRANSPORT_VERSION} CONFIG REQUIRED)
    message(STATUS "Transport: installed version: ${FireboltTransport_VERSION}, minimum required: ${FIREBOLT_TRANSPORT_VERSION}")
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)

if (ENABLE_DEMO_APP)
    add_subdirectory(test/api_test_app)
endif()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif()
