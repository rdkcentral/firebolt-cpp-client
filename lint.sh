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
MODE="check"
USE_DOCKER="auto"
IMAGE="${LINT_DOCKER_IMAGE:-firebolt-cpp-client-fmt:local}"

usage() {
  cat <<EOF
Usage: ./lint.sh [options]

Run the same formatting lint as CI for firebolt-cpp-client.

CI check command mirrored by this script:
  git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror

Options:
  --fix            Reformat files in place (same file set as CI)
  --check          Explicitly run check mode (default)
  --docker         Force Docker execution
  --local          Force local clang-format execution
  --image <name>   Docker image name (default: firebolt-cpp-client-fmt:local)
  --help           Show this help

Environment:
  SKIP_DOCKER=1    Same as --local

Examples:
  ./lint.sh
  ./lint.sh --fix
  ./lint.sh --docker
  SKIP_DOCKER=1 ./lint.sh
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --fix)
      MODE="fix"
      ;;
    --check)
      MODE="check"
      ;;
    --docker)
      USE_DOCKER="true"
      ;;
    --local)
      USE_DOCKER="false"
      ;;
    --image)
      if [[ $# -lt 2 || -z "${2:-}" || "$2" == --* ]]; then
        echo "Missing value for --image" >&2
        usage
        exit 1
      fi
      IMAGE="$2"
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

if [[ "${SKIP_DOCKER:-0}" == "1" ]]; then
  USE_DOCKER="false"
elif [[ "$USE_DOCKER" == "auto" ]]; then
  if command -v docker >/dev/null 2>&1; then
    USE_DOCKER="true"
  else
    USE_DOCKER="false"
  fi
fi

cd "$ROOT_DIR"

if [[ "$USE_DOCKER" == "true" ]]; then
  if ! docker image inspect "$IMAGE" >/dev/null 2>&1; then
    echo "[lint] Building Docker image '$IMAGE' with clang-format (one-time)"
    docker build -t "$IMAGE" - <<'DOCKERFILE'
FROM ubuntu:24.04
RUN apt-get update \
    && apt-get install -y --no-install-recommends clang-format git \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
DOCKERFILE
  fi

  if [[ "$MODE" == "fix" ]]; then
    echo "[lint] Running CI-equivalent clang-format file set in FIX mode via Docker"
    docker run --rm --user "$(id -u):$(id -g)" -v "$ROOT_DIR:/workspace" "$IMAGE" \
      bash -lc "set -e && git ls-files -- '*.cpp' '*.h' | xargs clang-format -i"
    echo "[lint] Formatting fixes applied"
  else
    echo "[lint] Running CI-equivalent clang-format check via Docker"
    docker run --rm --user "$(id -u):$(id -g)" -v "$ROOT_DIR:/workspace" "$IMAGE" \
      bash -lc "set -e && git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror"
    echo "[lint] Formatting OK"
  fi
else
  if ! command -v clang-format >/dev/null 2>&1; then
    echo "clang-format not found. Install it or run without SKIP_DOCKER=1." >&2
    exit 1
  fi

  echo "[lint] Using local clang-format: $(clang-format --version)"
  if [[ "$MODE" == "fix" ]]; then
    echo "[lint] Running CI-equivalent clang-format file set in FIX mode"
    git ls-files -- '*.cpp' '*.h' | xargs clang-format -i
    echo "[lint] Formatting fixes applied"
  else
    echo "[lint] Running CI-equivalent clang-format check"
    git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror
    echo "[lint] Formatting OK"
  fi
fi
