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

set -e

bdir="build"
run=true
buildType="Debug"

while [[ ! -z $1 ]]; do
  case $1 in
  --clean) rm -rf $bdir;;
  --just-run)
    export LD_LIBRARY_PATH=$bdir/src:$SYSROOT_PATH/usr/lib:$LD_LIBRARY_PATH
    $bdir/api_test_app -mock
    exit;;
  --no-run) run=false;;
  --release) buildType="Release";;
  --sysroot) SYSROOT_PATH="$2"; shift;;
  --) shift; break;;
  *) break;;
  esac; shift
done

[[ ! -z $SYSROOT_PATH ]] || { echo "SYSROOT_PATH not set" >/dev/stderr; exit 1; }
[[ -e $SYSROOT_PATH ]] || { echo "SYSROOT_PATH not exist ($SYSROOT_PATH)" >/dev/stderr; exit 1; }

if [[ ! -e $bdir ]]; then
  cmake -B $bdir \
    -DCMAKE_BUILD_TYPE=$buildType \
    -DSYSROOT_PATH=$SYSROOT_PATH \
    "$@"
else
  # Clean CMake cache if CMakeLists.txt is newer than cache
  if [[ CMakeLists.txt -nt $bdir/CMakeCache.txt ]]; then
    echo "CMakeLists.txt modified, cleaning cache..."
    rm -rf $bdir
    cmake -B $bdir \
      -DCMAKE_BUILD_TYPE=$buildType \
      -DSYSROOT_PATH=$SYSROOT_PATH \
      "$@"
  fi
fi
cmake --build $bdir

if $run; then
  export LD_LIBRARY_PATH=$bdir/src:$SYSROOT_PATH/usr/lib:$LD_LIBRARY_PATH
  $bdir/api_test_app -mock 
fi

