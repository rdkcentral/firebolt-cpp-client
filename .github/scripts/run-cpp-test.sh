#!/usr/bin/env bash
# vim: ts=4

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

set -eu

die() {
    echo "$@" >/dev/stderr
    exit 1
}

kill-rec() {
  local pid="$1" i= children=
  if children="$(pgrep -P "$pid")"; then
    for i in $children; do
      kill-rec $i
    done
  fi
  kill -9 "$pid"
}

specOpenRpc=
specAppOpenRpc=
mockPath=
mockConfig=
mockPort=9998
testExe=

while [[ -n ${1:-} ]]; do
    case $1 in
    --mock) mockPath="$2"; shift;;
    --port) mockPort="$2"; shift;;
    --config) mockConfig="$2"; shift;;
    --openrpc) specOpenRpc="$2"; shift;;
    --app-openrpc) specAppOpenRpc="$2"; shift;;
    --test-exe) testExe="$2"; shift;;
    *) break;;
    esac; shift
done

[[ -e $mockPath ]] || die "mock-firebolt not installed"
[[ -e $specOpenRpc ]] || die "OpenRPC spec '$specOpenRpc' not found"
[[ -e $specAppOpenRpc ]] || die "OpenRPC App spec '$specAppOpenRpc' not found"
[[ -e $mockConfig ]] || die "Config '$mockConfig' not found"
[[ -e $testExe ]] || die "Executable for cpp_test '$testExe' not found"

cfgFile=$mockPath/server/src/.mf.config.json

source $NVM_DIR/nvm.sh

echo "Updating config for mock, $cfgFile"
jq '
  (.supportedOpenRPCs[] | select(.name=="core")).fileName = "'"$specOpenRpc"'"
  | (.supportedToAppOpenRPCs[] | select(.name=="coreToApp")).fileName = "'"$specAppOpenRpc"'"
' \
  $mockConfig > $cfgFile

echo "Starting mock-server, $mockPath/server"
cd $mockPath/server
npm start &
mock_pid=$!
echo "Mock started at pid: $mock_pid"

sleep 1
try=0
maxTries=10
while ! nc -z localhost $mockPort >/dev/null 2>&1; do
  (( try < maxTries )) || die "Mock server not ready after $try tries"
  (( ++try ))
  printf "%d/%d Waiting for mock-server to be up&ready\n" $try $maxTries
  sleep 1
done

echo "Starting cpp_test with -auto flag"
cd "$(dirname "$testExe")"
"./$(basename "$testExe")" -auto
exitCode=$?

kill-rec $mock_pid

exit $exitCode
