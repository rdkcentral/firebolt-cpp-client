#!/usr/bin/env bash

# Copyright 2026 Comcast Cable Communications Management, LLC
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

# cov_build.sh — configure and build firebolt-cpp-client
#
# Run from the repo root after build_dependencies.sh has prepared the
# environment.  Produces a Debug build with tests enabled so that
# Coverity can intercept the full compilation including test code.
#
# Usage: ./cov_build.sh
set -euo pipefail
set -x

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GITHUB_WORKSPACE="${GITHUB_WORKSPACE:-${SCRIPT_DIR}}"
cd "${GITHUB_WORKSPACE}"

find_transport_config_dir() {
    local candidates=()
    local candidate
    local skip_system="${COV_SKIP_SYSTEM_TRANSPORT:-0}"

    if [[ -n "${FireboltTransport_DIR:-}" ]]; then
        candidates+=("${FireboltTransport_DIR}")
    fi

    if [[ -n "${COV_DEPS_PREFIX:-}" ]]; then
        candidates+=(
            "${COV_DEPS_PREFIX}/lib/cmake/FireboltTransport"
            "${COV_DEPS_PREFIX}/lib64/cmake/FireboltTransport"
        )
    fi

    if [[ "${skip_system}" != "1" ]]; then
        candidates+=(
            "/usr/local/lib/cmake/FireboltTransport"
            "/usr/local/lib64/cmake/FireboltTransport"
            "/usr/lib/cmake/FireboltTransport"
            "/usr/lib64/cmake/FireboltTransport"
            "/usr/lib/x86_64-linux-gnu/cmake/FireboltTransport"
        )
    fi

    for candidate in "${candidates[@]}"; do
        if [[ -f "${candidate}/FireboltTransportConfig.cmake" ]]; then
            echo "${candidate}"
            return 0
        fi
    done

    return 1
}

bootstrap_transport_if_missing() {
    local transport_repo="${GITHUB_WORKSPACE}/../firebolt-cpp-transport"
    local transport_build_dir="${transport_repo}/build-cov"
    local transport_src_dir=""
    local transport_version=""
    local release_dir=""
    local release_archive=""
    local release_url=""
    local release_url_nov=""
    local release_url_v=""
    local transport_config_dir=""
    local transport_cmake_args=()
    local force_bootstrap="${COV_FORCE_BOOTSTRAP_TRANSPORT:-0}"
    local force_release="${COV_FORCE_RELEASE_TRANSPORT:-0}"

    COV_DEPS_PREFIX="${COV_DEPS_PREFIX:-${GITHUB_WORKSPACE}/.cov-deps}"
    transport_src_dir="${COV_DEPS_PREFIX}/src"

    if [[ -f "${GITHUB_WORKSPACE}/.transport.version" ]]; then
        transport_version="$(tr -d '[:space:]' < "${GITHUB_WORKSPACE}/.transport.version")"
    fi

    if [[ "${force_bootstrap}" != "1" ]]; then
        if transport_config_dir="$(find_transport_config_dir)"; then
            echo "Using FireboltTransport from ${transport_config_dir}"
            FireboltTransport_DIR="${transport_config_dir}"
            return 0
        fi
    fi

    if [[ "${force_release}" == "1" ]]; then
        transport_repo=""
    fi

    if [[ ! -d "${transport_repo}" ]]; then
        if [[ ! -f "${GITHUB_WORKSPACE}/.transport.version" ]]; then
            echo "FireboltTransport not found and no .transport.version file is available." >&2
            echo "Provide FireboltTransport_DIR, add sibling firebolt-cpp-transport, or add .transport.version." >&2
            return 1
        fi

        if [[ -z "${transport_version}" ]]; then
            echo ".transport.version is empty; cannot resolve transport release." >&2
            return 1
        fi

        release_dir="firebolt-cpp-transport-${transport_version}"
        release_archive="${transport_src_dir}/${release_dir}.tar.gz"
        release_url_nov="https://github.com/rdkcentral/firebolt-cpp-transport/releases/download/v${transport_version}/${release_dir}.tar.gz"
        release_url_v="https://github.com/rdkcentral/firebolt-cpp-transport/releases/download/v${transport_version}/firebolt-cpp-transport-v${transport_version}.tar.gz"

        mkdir -p "${transport_src_dir}"

        if [[ ! -f "${release_archive}" ]]; then
            local tmp_archive="${release_archive}.tmp"
            rm -f "${tmp_archive}"
            if curl -fsSL --retry 5 --retry-delay 1 --retry-connrefused -o "${tmp_archive}" "${release_url_nov}"; then
                release_url="${release_url_nov}"
            elif curl -fsSL --retry 5 --retry-delay 1 --retry-connrefused -o "${tmp_archive}" "${release_url_v}"; then
                release_url="${release_url_v}"
            else
                rm -f "${tmp_archive}"
                echo "Failed to download FireboltTransport release for version ${transport_version}" >&2
                echo "Tried: ${release_url_nov}" >&2
                echo "Tried: ${release_url_v}" >&2
                return 1
            fi
            mv -f "${tmp_archive}" "${release_archive}"
            echo "Downloaded FireboltTransport release from ${release_url}"
        fi

        release_dir="$(tar -tzf "${release_archive}" | sed -e 's|^\./||' | awk -F/ 'NF{print $1; exit}')"
        if [[ -z "${release_dir}" ]]; then
            echo "Transport archive appears to be empty: ${release_archive}" >&2
            return 1
        fi

        rm -rf "${transport_src_dir:?}/${release_dir}"
        tar -xzf "${release_archive}" -C "${transport_src_dir}"

        transport_repo="${transport_src_dir}/${release_dir}"
        transport_build_dir="${transport_repo}/build-cov"

        if [[ ! -d "${transport_repo}" ]]; then
            echo "Transport source extraction failed: ${transport_repo} missing" >&2
            return 1
        fi
    fi

    if [[ -n "${transport_version}" ]]; then
        transport_cmake_args+=("-DFIREBOLT_TRANSPORT_VERSION=${transport_version}")
    fi

    cmake -S "${transport_repo}" -B "${transport_build_dir}" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX="${COV_DEPS_PREFIX}" \
        "${transport_cmake_args[@]}"

    cmake --build "${transport_build_dir}" --parallel
    cmake --install "${transport_build_dir}"

    if transport_config_dir="$(find_transport_config_dir)"; then
        FireboltTransport_DIR="${transport_config_dir}"
        echo "Bootstrapped FireboltTransport at ${FireboltTransport_DIR}"
        return 0
    fi

    echo "FireboltTransport bootstrap completed but config was not found." >&2
    return 1
}

bootstrap_transport_if_missing

cmake -B build-dev -S . \
    -UGTest_DIR \
    -DCMAKE_BUILD_TYPE=Debug \
    -DFireboltTransport_DIR="${FireboltTransport_DIR}" \
    -DENABLE_TESTS=ON

cmake --build build-dev --parallel
