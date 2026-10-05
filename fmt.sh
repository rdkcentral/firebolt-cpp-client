#!/usr/bin/env bash
# Check (default) or fix clang-format. Like `cargo fmt [--check]`.
#   ./fmt.sh        — check only (exit 1 if violations)
#   ./fmt.sh --fix  — reformat in place
#
# Uses Docker by default (matches CI exactly). If Docker is unavailable or
# SKIP_DOCKER=1 is set, falls back to the locally installed clang-format.
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Minimal image — only clang-format, matches the Ubuntu version used in CI
IMAGE="firebolt-cpp-client-fmt:local"

use_docker=true
if [[ "${SKIP_DOCKER:-0}" == "1" ]] || ! command -v docker &>/dev/null; then
    use_docker=false
fi

if [[ "$use_docker" == true ]]; then
    if ! docker image inspect "$IMAGE" &>/dev/null; then
        echo "Building clang-format Docker image (one-time, ~30s)..."
        docker build -t "$IMAGE" - <<'DOCKERFILE'
FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends clang-format git && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
DOCKERFILE
    fi
    RUN="docker run --rm --user $(id -u):$(id -g) -v $SCRIPT_DIR:/workspace $IMAGE bash -c"
else
    if ! command -v clang-format &>/dev/null; then
        echo "clang-format not found. Install it or run without SKIP_DOCKER=1." >&2
        exit 1
    fi
    echo "[fmt] Using local clang-format ($(clang-format --version))"
    RUN="bash -c"
fi

if [[ "${1:-}" == "--fix" ]]; then
    $RUN "set -e && git ls-files -- '*.cpp' '*.h' | xargs clang-format -i"
    echo "Done. Files reformatted."
else
    $RUN "set -e && git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror"
    echo "Formatting OK."
fi
