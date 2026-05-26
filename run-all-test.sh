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
RUN_UNIT=true
RUN_COMPONENT=true
BUILD_ONLY=false
CLEAN=false
UNIT_FILTER=""
COMPONENT_FILTER=""

usage() {
	cat <<EOF
Usage: ./run-all-test.sh [options]

Delegates to ./run-unit-tests.sh and ./run-component-tests.sh.

Options:
	--clean                      Remove build-dev before building
	--build-only                 Build tests only; do not run
	--unit-only                  Run unit tests only (utApp)
	--component-only             Run component tests only (ctApp)
	--unit-filter <gtest-filter>  GTest filter passed to utApp
	--component-filter <filter>   GTest filter passed to ctApp
	--help                        Show this help

Examples:
	./run-all-test.sh
	./run-all-test.sh --unit-only --unit-filter "ActionsUTest.*"
	./run-all-test.sh --component-only --component-filter "*Localization*"
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
		--unit-only)
			RUN_COMPONENT=false
			;;
		--component-only)
			RUN_UNIT=false
			;;
		--unit-filter)
			UNIT_FILTER="${2:-}"
			shift
			;;
		--component-filter)
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

if [[ "$RUN_UNIT" == false && "$RUN_COMPONENT" == false ]]; then
	echo "Nothing to run: both unit and component test execution are disabled." >&2
	exit 1
fi

run_unit_script="$ROOT_DIR/run-unit-tests.sh"
run_component_script="$ROOT_DIR/run-component-tests.sh"

bootstrap_args=()
if [[ "$CLEAN" == true ]]; then
	bootstrap_args+=(--clean)
fi

if [[ "$RUN_UNIT" == true && "$RUN_COMPONENT" == true ]]; then
	echo "[run-all-test] Bootstrapping build once via run-unit-tests.sh --build-only"
	"$run_unit_script" "${bootstrap_args[@]}" --build-only

	if [[ "$BUILD_ONLY" == true ]]; then
		echo "Build complete. Skipping test execution (--build-only)."
		exit 0
	fi

	unit_args=(--no-build)
	if [[ -n "$UNIT_FILTER" ]]; then
		unit_args+=(--unit-filter "$UNIT_FILTER")
	fi
	echo "[run-all-test] Running unit tests via run-unit-tests.sh"
	"$run_unit_script" "${unit_args[@]}"

	component_args=(--no-build)
	if [[ -n "$COMPONENT_FILTER" ]]; then
		component_args+=(--component-filter "$COMPONENT_FILTER")
	fi
	echo "[run-all-test] Running component tests via run-component-tests.sh"
	"$run_component_script" "${component_args[@]}"
	exit 0
fi

if [[ "$RUN_UNIT" == true ]]; then
	unit_args=()
	if [[ "$CLEAN" == true ]]; then
		unit_args+=(--clean)
	fi
	if [[ "$BUILD_ONLY" == true ]]; then
		unit_args+=(--build-only)
	fi
	if [[ -n "$UNIT_FILTER" ]]; then
		unit_args+=(--unit-filter "$UNIT_FILTER")
	fi
	echo "[run-all-test] Delegating to run-unit-tests.sh"
	"$run_unit_script" "${unit_args[@]}"
	exit 0
fi

component_args=()
if [[ "$CLEAN" == true ]]; then
	component_args+=(--clean)
fi
if [[ "$BUILD_ONLY" == true ]]; then
	component_args+=(--build-only)
fi
if [[ -n "$COMPONENT_FILTER" ]]; then
	component_args+=(--component-filter "$COMPONENT_FILTER")
fi
echo "[run-all-test] Delegating to run-component-tests.sh"
"$run_component_script" "${component_args[@]}"
