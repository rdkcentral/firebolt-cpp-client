#!/usr/bin/env bash

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

set -e

bdir="build"
do_install=false
params=
buildType="Debug"
cleanFirst=false

while [[ ! -z $1 ]]; do
  case $1 in
  --clean) cleanFirst=true;;
  --release) buildType="Release";;
  --sysroot) SYSROOT_PATH="$2"; shift;;
  -i | --install) do_install=true;;
  +tests) params+=" -DENABLE_TESTS=ON"; bdir="build-dev";;
  +demo)  params+=" -DENABLE_DEMO_APP=ON";;
  +gen-cov)
    set -e
    cd build-dev
    cp ../firebolt-open-rpc.json ./test/
    ctest --test-dir ./test
    mkdir -p coverage
    gcovr -r .. \
      --exclude '.*/test/.*\.h' \
      --exclude '.*/test/.*\.cpp' \
      --decisions \
      --medium-threshold 50 --high-threshold 75 \
      --html-details coverage/index.html \
      --cobertura coverage.cobertura.xml
    exit 0;;
  --) shift; break;;
  *) break;;
  esac; shift
done

if [[ -n "$SYSROOT_PATH" ]]; then
  [[ -e "$SYSROOT_PATH" ]] || { echo "SYSROOT_PATH not exist ($SYSROOT_PATH)" >/dev/stderr; exit 1; }
  params+=" -DSYSROOT_PATH=$SYSROOT_PATH"
else
  if $do_install; then
    echo "--install requires --sysroot to be set; refusing to install without SYSROOT_PATH" >/dev/stderr
    exit 1
  fi
  echo "SYSROOT_PATH not set; building without SYSROOT_PATH override"
fi

if [[ "$do_install" == true && "$bdir" == "build" && -z "${SYSROOT_PATH:-}" ]]; then
  echo "Refusing --install without --sysroot to avoid host install into /usr" >&2
  exit 1
fi

$cleanFirst && rm -rf $bdir

if [[ ! -e "$bdir" || -n "$@" ]]; then
  params+=" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
  command -v ccache >/dev/null 2>&1 && params+=" -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
  cmake -B $bdir \
    -DCMAKE_BUILD_TYPE=$buildType \
    $params \
    "$@" || exit $?
fi
cmake --build $bdir --parallel || exit $?
if $do_install && [[ $bdir == 'build' ]]; then
  cmake --install $bdir || exit $?
fi
