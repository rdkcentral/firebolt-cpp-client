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

if (NOT PROJECT_VERSION)
    set(VERSION_STRING "0.1.0-unknown")

    find_package(Git QUIET)

    if (GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0 --match "v[0-9]*.[0-9]*.[0-9]*"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif ()

    if (GIT_VERSION)
        string(REGEX REPLACE "^v" "" VERSION_STRING "${GIT_VERSION}")
    endif ()

    if(VERSION_STRING STREQUAL "0.1.0-unknown" AND EXISTS "${CMAKE_SOURCE_DIR}/.version")
        file(READ "${CMAKE_SOURCE_DIR}/.version" VERSION_STRING)
        string(STRIP "${VERSION_STRING}" VERSION_STRING)
    endif()

    set(PROJECT_VERSION "${VERSION_STRING}" CACHE STRING "Project version string")
    set(PROJECT_VERSION "${VERSION_STRING}")
endif ()

set(VERSION ${PROJECT_VERSION})
string(REGEX REPLACE "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\1" PROJECT_VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\2" PROJECT_VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+).*" "\\3" PROJECT_VERSION_PATCH "${VERSION}")

function(check_version_compatibility result_var found_version required_version)
    set(FOUND_VERSION_SUFFIX "")
    set(FOUND_VERSION_SUFFIX_NO "")
    string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-(.*))?" "\\1" FOUND_VERSION_MAJOR "${found_version}")
    if (found_version MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)-([^.]*)(\\.(.*))?")
      set(FOUND_VERSION_SUFFIX "${CMAKE_MATCH_4}")
      set(FOUND_VERSION_SUFFIX_NO "${CMAKE_MATCH_6}")
    endif()

    set(REQUIRED_VERSION_SUFFIX "")
    set(REQUIRED_VERSION_SUFFIX_NO "")
    string(REGEX REPLACE "^([0-9]+)(\\.([0-9]+)(\\.([0-9]+))?)?(-(.*))?" "\\1" REQUIRED_VERSION_MAJOR "${required_version}")
    if (required_version MATCHES "^([0-9]+)(\\.[0-9]+(\\.[0-9]+)?)?-([^.]*)(\\.([0-9]+))?")
      set(REQUIRED_VERSION_SUFFIX "${CMAKE_MATCH_4}")
      set(REQUIRED_VERSION_SUFFIX_NO "${CMAKE_MATCH_6}")
    endif()

    set(IS_OK FALSE)
    if (NOT FOUND_VERSION_MAJOR EQUAL REQUIRED_VERSION_MAJOR)
        set(IS_OK FALSE)
    elseif(found_version VERSION_LESS required_version)
        set(IS_OK FALSE)
    elseif(REQUIRED_VERSION_SUFFIX STREQUAL "" AND FOUND_VERSION_SUFFIX STREQUAL "")
        set(IS_OK TRUE)
    elseif(FOUND_VERSION_SUFFIX STREQUAL REQUIRED_VERSION_SUFFIX)
        if(REQUIRED_VERSION_SUFFIX_NO STREQUAL "" OR FOUND_VERSION_SUFFIX_NO GREATER_EQUAL REQUIRED_VERSION_SUFFIX_NO)
            set(IS_OK TRUE)
        endif()
    endif()

    set(${result_var} ${IS_OK} PARENT_SCOPE)
endfunction()
