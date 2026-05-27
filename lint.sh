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
NO_BUILD=false
CLEAN=false
RUN_CLANG_TIDY=true
RUN_CPPCHECK=true
APPLY_FIXES=false
CLANG_TIDY_PATHS=(src include test/unit test/component)

usage() {
  cat <<EOF
Usage: ./lint.sh [options]

Runs C/C++ lint checks for firebolt-cpp-client.
Default behavior: build compile_commands.json via ./build.sh +tests, then run
clang-tidy and cppcheck.

Options:
  --clean           Remove build directory before building
  --no-build        Skip build step and use existing compile database
  --build-dir <dir> Build directory containing compile_commands.json (default: build-dev)
  --tidy-path <p>   Add path for clang-tidy scan (repeatable)
  --fix             Apply clang-tidy fix-its (clang-tidy only)
  --tidy-only       Run clang-tidy only
  --cppcheck-only   Run cppcheck only
  --help            Show this help

Examples:
  ./lint.sh
  ./lint.sh --tidy-only
  ./lint.sh --tidy-only --fix
  ./lint.sh --tidy-path test/api_test_app
  ./lint.sh --no-build --build-dir build-dev
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --clean)
      CLEAN=true
      ;;
    --no-build)
      NO_BUILD=true
      ;;
    --build-dir)
      if [[ $# -lt 2 || -z "${2:-}" || "${2:0:1}" == "-" ]]; then
        echo "Missing value for --build-dir" >&2
        usage
        exit 1
      fi
      BUILD_DIR="${2:-}"
      shift
      ;;
    --tidy-path)
      if [[ $# -lt 2 || -z "${2:-}" || "${2:0:1}" == "-" ]]; then
        echo "Missing value for --tidy-path" >&2
        usage
        exit 1
      fi
      CLANG_TIDY_PATHS+=("${2:-}")
      shift
      ;;
    --fix)
      APPLY_FIXES=true
      ;;
    --tidy-only)
      RUN_CLANG_TIDY=true
      RUN_CPPCHECK=false
      ;;
    --cppcheck-only)
      RUN_CLANG_TIDY=false
      RUN_CPPCHECK=true
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

if [[ "$RUN_CLANG_TIDY" == false && "$RUN_CPPCHECK" == false ]]; then
  echo "Nothing to run: both clang-tidy and cppcheck are disabled." >&2
  exit 1
fi

if [[ "$APPLY_FIXES" == true && "$RUN_CLANG_TIDY" == false ]]; then
  echo "--fix requires clang-tidy to be enabled (remove --cppcheck-only)." >&2
  exit 1
fi

if [[ "$RUN_CLANG_TIDY" == true ]] && ! command -v clang-tidy >/dev/null 2>&1; then
  echo "clang-tidy not found. Install it (e.g. apt install clang-tidy)." >&2
  exit 1
fi

if [[ "$RUN_CPPCHECK" == true ]] && ! command -v cppcheck >/dev/null 2>&1; then
  echo "cppcheck not found. Install it (e.g. apt install cppcheck)." >&2
  exit 1
fi

if [[ "$CLEAN" == true ]]; then
  rm -rf "$BUILD_DIR"
fi

if [[ "$NO_BUILD" == false ]]; then
  ./build.sh +tests
fi

if [[ ! -f "$BUILD_DIR/compile_commands.json" ]]; then
  echo "Missing $BUILD_DIR/compile_commands.json. Run ./build.sh +tests first." >&2
  exit 1
fi

if [[ "$RUN_CLANG_TIDY" == true ]]; then
  if [[ "$APPLY_FIXES" == true ]]; then
    echo "[lint] Running clang-tidy with fixes enabled"
  else
    echo "[lint] Running clang-tidy"
  fi

  existing_paths=()
  for p in "${CLANG_TIDY_PATHS[@]}"; do
    if [[ -e "$p" ]]; then
      existing_paths+=("$p")
    fi
  done

  if [[ ${#existing_paths[@]} -eq 0 ]]; then
    echo "No valid clang-tidy paths found." >&2
    exit 1
  fi

  mapfile -t source_files < <(
    find "${existing_paths[@]}" -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" \) | sort
  )

  if [[ ${#source_files[@]} -eq 0 ]]; then
    echo "No C/C++ source files found for clang-tidy." >&2
    exit 1
  fi

  clang_tidy_failed=0
  total_files=${#source_files[@]}
  index=0
  for f in "${source_files[@]}"; do
    index=$((index + 1))
    echo "[lint][clang-tidy] ${index}/${total_files}: $f"
    clang_tidy_cmd=(clang-tidy -p "$BUILD_DIR")
    if [[ "$APPLY_FIXES" == true ]]; then
      clang_tidy_cmd+=("-fix")
    fi
    clang_tidy_cmd+=("$f")
    if ! "${clang_tidy_cmd[@]}"; then
      clang_tidy_failed=1
    fi
  done

  if [[ $clang_tidy_failed -ne 0 ]]; then
    echo "clang-tidy reported issues." >&2
    exit 1
  fi
fi

if [[ "$RUN_CPPCHECK" == true ]]; then
  echo "[lint] Running cppcheck"
  cppcheck \
    --enable=warning,style,performance,portability \
    --std=c++17 \
    --language=c++ \
    --inline-suppr \
    --error-exitcode=1 \
    -I include \
    -I src \
    src include test
fi

echo "[lint] Completed successfully"
