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

LOCAL_PORT="3474"
REMOTE_PORT="3474"
REMOTE_BIND_ADDR="127.0.0.1"
BACKGROUND=false

require_env() {
  local name="$1"
  local value="${!name:-}"
  if [[ -z "$value" ]]; then
    echo "Error: required environment variable $name is not set" >&2
    exit 1
  fi
}

usage() {
  cat <<EOF
Usage: ./setup-device-tunnel.sh [options]

Create an SSH local-forward tunnel for Firebolt websocket traffic:
  localhost:<local-port> -> <remote-bind-addr>:<remote-port> on <remote-host>

Required environment variables:
  DEVICE_SSH_USER   SSH username
  DEVICE_SSH_HOST   Remote SSH host
  DEVICE_SSH_PORT   SSH port

Defaults:
  local-port        ${LOCAL_PORT}
  remote-port       ${REMOTE_PORT}
  remote-bind-addr  ${REMOTE_BIND_ADDR}

Options:
  --local-port <port>    Local forwarded port (default: ${LOCAL_PORT})
  --remote-port <port>   Remote websocket port (default: ${REMOTE_PORT})
  --remote-bind <addr>   Remote bind address (default: ${REMOTE_BIND_ADDR})
  --background           Run ssh tunnel in background
  --help                 Show this help

Examples:
  export DEVICE_SSH_USER=root
  export DEVICE_SSH_HOST=192.168.201.170
  export DEVICE_SSH_PORT=10022
  ./setup-device-tunnel.sh

After tunnel is up, run component tests with:
  FIREBOLT_ENDPOINT=ws://127.0.0.1:${LOCAL_PORT}/ ./run-component-tests.sh
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --local-port)
      if [[ $# -lt 2 || -z "${2:-}" || "${2:0:1}" == "-" ]]; then
        echo "Missing value for --local-port" >&2
        usage
        exit 1
      fi
      LOCAL_PORT="${2:-}"
      shift
      ;;
    --remote-port)
      if [[ $# -lt 2 || -z "${2:-}" || "${2:0:1}" == "-" ]]; then
        echo "Missing value for --remote-port" >&2
        usage
        exit 1
      fi
      REMOTE_PORT="${2:-}"
      shift
      ;;
    --remote-bind)
      if [[ $# -lt 2 || -z "${2:-}" || "${2:0:1}" == "-" ]]; then
        echo "Missing value for --remote-bind" >&2
        usage
        exit 1
      fi
      REMOTE_BIND_ADDR="${2:-}"
      shift
      ;;
    --background)
      BACKGROUND=true
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

require_env DEVICE_SSH_USER
require_env DEVICE_SSH_HOST
require_env DEVICE_SSH_PORT

TARGET="${DEVICE_SSH_USER}@${DEVICE_SSH_HOST}"
FORWARD_SPEC="${LOCAL_PORT}:${REMOTE_BIND_ADDR}:${REMOTE_PORT}"
SSH_ARGS=(-N -o BatchMode=yes -o StrictHostKeyChecking=accept-new -o ExitOnForwardFailure=yes -p "$DEVICE_SSH_PORT" -L "$FORWARD_SPEC" "$TARGET")

echo "Opening SSH tunnel: localhost:${LOCAL_PORT} -> ${REMOTE_BIND_ADDR}:${REMOTE_PORT} on ${TARGET}"

if [[ "$BACKGROUND" == true ]]; then
  # -f backgrounds only after authentication and forwarding are set up.
  ssh -f "${SSH_ARGS[@]}"
  echo "Tunnel established in background mode."
  exit 0
fi

exec ssh "${SSH_ARGS[@]}"
