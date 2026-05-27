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

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="build-dev"
BUILD_ONLY=false
NO_BUILD=false
CLEAN=false
UNIT_FILTER=""

usage() {
  cat <<EOF
Usage: ./run-unit-tests.sh [options]

Builds test binaries via ./build.sh +tests, then runs utApp only.

Options:
  --clean                      Remove build-dev before building
  --build-only                 Build tests only; do not run
  --no-build                    Skip building; run utApp from build-dev/test
  --unit-filter <gtest-filter> GTest filter passed to utApp
  --help                       Show this help

Examples:
  ./run-unit-tests.sh
  ./run-unit-tests.sh --unit-filter "ActionsUTest.*"
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --clean)
      CLEAN=true
      ;;
    --build-only)
      BUILD_ONLY=true
      ;;
    --no-build)
      NO_BUILD=true
      ;;
    --unit-filter)
      if [[ $# -lt 2 || -z "${2:-}" || "${2:0:1}" == "-" ]]; then
        echo "Missing value for --unit-filter" >&2
        usage
        exit 1
      fi
      UNIT_FILTER="${2:-}"
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage
      exit 1
      ;;
  esac
  shift
done

cd "$ROOT_DIR"

if [[ "$CLEAN" == true ]]; then
  rm -rf "$BUILD_DIR"
fi

if [[ "$NO_BUILD" == false ]]; then
  ./build.sh +tests
fi

if [[ "$BUILD_ONLY" == true ]]; then
  echo "Build complete. Skipping test execution (--build-only)."
  exit 0
fi

cd "$BUILD_DIR/test"
export LD_LIBRARY_PATH="../src:${LD_LIBRARY_PATH:-}"

unit_cmd=(./utApp)
if [[ -n "$UNIT_FILTER" ]]; then
  unit_cmd+=("--gtest_filter=$UNIT_FILTER")
fi
echo "[run-unit-tests] Running unit tests: ${unit_cmd[*]}"
"${unit_cmd[@]}"