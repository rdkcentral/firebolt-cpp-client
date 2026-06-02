#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PROTOCOL="rpc_v2"
MOCK_DIR="/tmp/mock-firebolt-firebolt-cpp-client"
IMAGE_TAG="firebolt-client-ci:local"
SKIP_IMAGE_BUILD="false"

usage() {
  cat <<EOF
Usage: ./run-component-tests-docker.sh [options]

One-shot Docker flow:
1) Build CI image (unless --skip-image-build)
2) Clone/update mock-firebolt into a host directory
3) Build ctApp in container
4) Run component tests through .github/scripts/run-component-tests.sh

Options:
  --protocol <rpc_v2|legacy>   RPC protocol (default: rpc_v2)
  --mock-dir <path>            Host directory for mock-firebolt cache
                               (default: /tmp/mock-firebolt-firebolt-cpp-client)
  --image-tag <tag>            Docker image tag to use/build
                               (default: firebolt-client-ci:local)
  --skip-image-build           Reuse existing image tag; do not run docker build
  --help                       Show this help

Examples:
  ./run-component-tests-docker.sh
  ./run-component-tests-docker.sh --protocol legacy
  ./run-component-tests-docker.sh --skip-image-build --image-tag firebolt-client-ci:local
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --protocol)
      [[ $# -ge 2 ]] || { echo "Missing value for --protocol" >&2; exit 1; }
      PROTOCOL="$2"
      shift
      ;;
    --mock-dir)
      [[ $# -ge 2 ]] || { echo "Missing value for --mock-dir" >&2; exit 1; }
      MOCK_DIR="$2"
      shift
      ;;
    --image-tag)
      [[ $# -ge 2 ]] || { echo "Missing value for --image-tag" >&2; exit 1; }
      IMAGE_TAG="$2"
      shift
      ;;
    --skip-image-build)
      SKIP_IMAGE_BUILD="true"
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

if [[ "$PROTOCOL" != "rpc_v2" && "$PROTOCOL" != "legacy" ]]; then
  echo "Invalid --protocol '$PROTOCOL'. Expected 'rpc_v2' or 'legacy'." >&2
  exit 1
fi

cd "$ROOT_DIR"

if [[ ! -f .transport.version ]]; then
  echo "Missing .transport.version in $ROOT_DIR" >&2
  exit 1
fi

TRANSPORT_VERSION="$(cat .transport.version)"
MOCK_SHA1SUM="1fec7b75190e75ac8ea8ebf9f3e00c0a070b2566"
MOCK_BRANCH="topic/changes-for-bidirectional"
NODE_VERSION="24.11.0"

mkdir -p "$MOCK_DIR"

echo "[1/4] Docker image: $IMAGE_TAG"
if [[ "$SKIP_IMAGE_BUILD" == "false" ]]; then
  docker build \
    -f "$ROOT_DIR/.github/Dockerfile" \
    -t "$IMAGE_TAG" \
    --build-arg "DEPS_TRANSPORT_V=$TRANSPORT_VERSION" \
    --build-arg "DEPS_TRANSPORT_PROTOCOL=$PROTOCOL" \
    "$ROOT_DIR"
else
  echo "Skipping image build (--skip-image-build)"
fi

echo "[2/4] Preparing mock-firebolt in $MOCK_DIR"
docker run --rm --user "$(id -u):$(id -g)" \
  -v "$ROOT_DIR:/workspace" \
  -v "$MOCK_DIR:/mock-host" \
  "$IMAGE_TAG" \
  bash -c '
    set -e
    if [ ! -d /mock-host/.git ]; then
      git clone --depth 1 --branch '"$MOCK_BRANCH"' \
        https://github.com/rdkcentral/mock-firebolt.git /mock-host
    fi
    cd /mock-host
    git fetch --shallow-since=2026-01-01
    git -c advice.detachedHead=false checkout '"$MOCK_SHA1SUM"'
    source /usr/local/nvm/nvm.sh
    nvm use --delete-prefix '"$NODE_VERSION"' >/dev/null
    cd server
    npm ci
  '

BUILD_SUBDIR="build-docker"

echo "[3/4] Building ctApp (build dir: $BUILD_SUBDIR)"
docker run --rm --user "$(id -u):$(id -g)" \
  -v "$ROOT_DIR:/workspace" \
  "$IMAGE_TAG" \
  bash -c '
    set -e
    cd /workspace
    rm -rf '"$BUILD_SUBDIR"'
    cmake -B '"$BUILD_SUBDIR"' -S . -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
    cp docs/openrpc/the-spec/firebolt-open-rpc.json '"$BUILD_SUBDIR"'/test/
    cmake --build '"$BUILD_SUBDIR"' --parallel
    chmod +x '"$BUILD_SUBDIR"'/test/ctApp
  '

echo "[4/4] Running component tests"
docker run --rm --user "$(id -u):$(id -g)" \
  -v "$ROOT_DIR:/workspace" \
  -v "$MOCK_DIR:/mock" \
  "$IMAGE_TAG" \
  /workspace/.github/scripts/run-component-tests.sh \
    --mock /mock \
    --protocol "$PROTOCOL" \
    --config /workspace/.github/mock-firebolt/config.json \
    --openrpc /workspace/docs/openrpc/the-spec/firebolt-open-rpc.json \
    --app-openrpc /workspace/docs/openrpc/the-spec/firebolt-app-open-rpc.json \
    --test-exe /workspace/"$BUILD_SUBDIR"/test/ctApp

echo "Done."
