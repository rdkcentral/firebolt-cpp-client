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
COMPONENT_FILTER=""

usage() {
  cat <<EOF
Usage: ./run-component-tests.sh [options]

Builds test binaries via ./build.sh +tests, then runs ctApp only.

Options:
  --clean                       Remove build-dev before building
  --build-only                  Build tests only; do not run
  --no-build                    Skip building; run ctApp from build-dev/test
  --component-filter <filter>   GTest filter passed to ctApp
  --help                        Show this help

Examples:
  ./run-component-tests.sh
  FIREBOLT_ENDPOINT=ws://127.0.0.1:3474/ ./run-component-tests.sh --component-filter "*Localization*"
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
    --component-filter)
      if [[ $# -lt 2 || -z "${2:-}" || "$2" == --* ]]; then
        echo "Missing value for --component-filter" >&2
        usage
        exit 1
      fi
      COMPONENT_FILTER="${2:-}"
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

component_cmd=(./ctApp)
if [[ -n "$COMPONENT_FILTER" ]]; then
  component_cmd+=("--gtest_filter=$COMPONENT_FILTER")
fi
echo "[run-component-tests] Running component tests: ${component_cmd[*]}"
"${component_cmd[@]}"