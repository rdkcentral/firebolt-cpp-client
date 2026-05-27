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

die() {
  echo "Error: $*" >&2
  exit 1
}

require_cmd() {
  local cmd="$1"
  local install_hint="$2"
  if ! command -v "$cmd" >/dev/null 2>&1; then
    die "Missing required dependency '$cmd'. ${install_hint}"
  fi
}

preflight_checks() {
  if [[ -z "${BASH_VERSION:-}" ]]; then
    die "This script must be run with bash. Example: bash ./resolve-review-threads.sh <pr-number>"
  fi

  if (( BASH_VERSINFO[0] < 4 )); then
    die "Bash 4+ is required (mapfile is used). Current bash version: ${BASH_VERSION}"
  fi

  require_cmd "gh" "Install GitHub CLI: https://cli.github.com/"
  require_cmd "jq" "Install jq (for example: sudo apt-get install jq)"

  if ! gh auth status >/dev/null 2>&1; then
    die "GitHub CLI is not authenticated. Run: gh auth login"
  fi

  if ! gh api graphql -f query='query { viewer { login } }' >/dev/null 2>&1; then
    die "GitHub GraphQL API call failed. Check network connectivity and token permissions for gh auth."
  fi
}

usage() {
  cat <<'EOF'
Usage: ./resolve-review-threads.sh <pr-number> [options]

Resolves unresolved PR review threads that you are allowed to resolve.

Options:
  --repo <owner/repo>  Repository to target. Default: current gh repo
  --all-unresolved     Resolve all unresolved resolvable threads
  --dry-run            Print what would be resolved without mutating anything
  --help               Show this help

Examples:
  ./resolve-review-threads.sh 63
  ./resolve-review-threads.sh 63 --all-unresolved
  ./resolve-review-threads.sh 63 --repo rdkcentral/firebolt-cpp-client
EOF
}

# Allow usage output in unconfigured environments.
for arg in "$@"; do
  if [[ "$arg" == "--help" || "$arg" == "-h" ]]; then
    usage
    exit 0
  fi
done

preflight_checks

if [[ $# -lt 1 ]]; then
  usage
  exit 1
fi

PR_NUMBER=""
REPO=""
OUTDATED_ONLY=false
RESOLVE_ALL=false
DRY_RUN=false

if [[ "$1" =~ ^[0-9]+$ ]]; then
  PR_NUMBER="$1"
  shift
else
  echo "Error: first argument must be a PR number." >&2
  usage
  exit 1
fi

while [[ $# -gt 0 ]]; do
  case "$1" in
    --repo)
      if [[ $# -lt 2 || -z "${2:-}" || "${2:0:1}" == "-" ]]; then
        echo "Error: --repo requires owner/repo." >&2
        exit 1
      fi
      REPO="$2"
      shift 2
      ;;
    --all-unresolved)
      RESOLVE_ALL=true
      shift
      ;;
    --dry-run)
      DRY_RUN=true
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "Error: unknown option: $1" >&2
      usage
      exit 1
      ;;
  esac
done

# Default mode is outdated-only unless explicitly overridden.
if [[ "$RESOLVE_ALL" == "true" ]]; then
  OUTDATED_ONLY=false
else
  OUTDATED_ONLY=true
fi

if [[ -z "$REPO" ]]; then
  REPO="$(gh repo view --json nameWithOwner -q .nameWithOwner)"
fi

if [[ "$REPO" != */* ]]; then
  die "Invalid repository value '$REPO'. Expected format: owner/repo"
fi

OWNER="${REPO%%/*}"
NAME="${REPO##*/}"

QUERY_FIRST='query($owner:String!, $name:String!, $number:Int!) {
  repository(owner:$owner, name:$name) {
    pullRequest(number:$number) {
      reviewThreads(first:100) {
        pageInfo { hasNextPage endCursor }
        nodes {
          id
          isResolved
          isOutdated
          viewerCanResolve
        }
      }
    }
  }
}'

QUERY_NEXT='query($owner:String!, $name:String!, $number:Int!, $cursor:String!) {
  repository(owner:$owner, name:$name) {
    pullRequest(number:$number) {
      reviewThreads(first:100, after:$cursor) {
        pageInfo { hasNextPage endCursor }
        nodes {
          id
          isResolved
          isOutdated
          viewerCanResolve
        }
      }
    }
  }
}'

MUTATION_RESOLVE='mutation($id:ID!) {
  resolveReviewThread(input:{threadId:$id}) {
    thread { id isResolved }
  }
}'

cursor=""
has_next=true

declare -a THREAD_IDS=()
total_threads=0
unresolved_threads=0
unresolved_outdated_threads=0
unresolved_resolvable_threads=0
unresolved_unresolvable_threads=0

while [[ "$has_next" == "true" ]]; do
  if [[ -z "$cursor" ]]; then
    resp="$(gh api graphql -f query="$QUERY_FIRST" -F owner="$OWNER" -F name="$NAME" -F number="$PR_NUMBER")"
  else
    resp="$(gh api graphql -f query="$QUERY_NEXT" -F owner="$OWNER" -F name="$NAME" -F number="$PR_NUMBER" -F cursor="$cursor")"
  fi

  if [[ "$(echo "$resp" | jq -r '.data.repository.pullRequest == null')" == "true" ]]; then
    echo "Error: PR #$PR_NUMBER not found in $REPO" >&2
    exit 1
  fi

  page_total="$(echo "$resp" | jq -r '.data.repository.pullRequest.reviewThreads.nodes | length')"
  page_unresolved="$(echo "$resp" | jq -r '[.data.repository.pullRequest.reviewThreads.nodes[] | select(.isResolved | not)] | length')"
  page_unresolved_outdated="$(echo "$resp" | jq -r '[.data.repository.pullRequest.reviewThreads.nodes[] | select((.isResolved | not) and (.isOutdated == true))] | length')"
  page_unresolved_resolvable="$(echo "$resp" | jq -r '[.data.repository.pullRequest.reviewThreads.nodes[] | select((.isResolved | not) and (.viewerCanResolve == true))] | length')"
  page_unresolved_unresolvable="$(echo "$resp" | jq -r '[.data.repository.pullRequest.reviewThreads.nodes[] | select((.isResolved | not) and (.viewerCanResolve == false))] | length')"

  total_threads=$((total_threads + page_total))
  unresolved_threads=$((unresolved_threads + page_unresolved))
  unresolved_outdated_threads=$((unresolved_outdated_threads + page_unresolved_outdated))
  unresolved_resolvable_threads=$((unresolved_resolvable_threads + page_unresolved_resolvable))
  unresolved_unresolvable_threads=$((unresolved_unresolvable_threads + page_unresolved_unresolvable))

  if [[ "$OUTDATED_ONLY" == "true" ]]; then
    mapfile -t page_ids < <(echo "$resp" | jq -r '.data.repository.pullRequest.reviewThreads.nodes[] | select((.isResolved | not) and (.viewerCanResolve == true) and (.isOutdated == true)) | .id')
  else
    mapfile -t page_ids < <(echo "$resp" | jq -r '.data.repository.pullRequest.reviewThreads.nodes[] | select((.isResolved | not) and (.viewerCanResolve == true)) | .id')
  fi

  if [[ ${#page_ids[@]} -gt 0 ]]; then
    THREAD_IDS+=("${page_ids[@]}")
  fi

  has_next="$(echo "$resp" | jq -r '.data.repository.pullRequest.reviewThreads.pageInfo.hasNextPage')"
  cursor="$(echo "$resp" | jq -r '.data.repository.pullRequest.reviewThreads.pageInfo.endCursor // ""')"
done

if [[ ${#THREAD_IDS[@]} -eq 0 ]]; then
  echo "Target repo: $REPO"
  echo "PR: #$PR_NUMBER"
  echo "Threads total: $total_threads"
  echo "Unresolved: $unresolved_threads"
  echo "Unresolved outdated: $unresolved_outdated_threads"
  echo "Unresolved resolvable by current user: $unresolved_resolvable_threads"
  echo "Unresolved NOT resolvable by current user: $unresolved_unresolvable_threads"

  if [[ "$unresolved_threads" -gt 0 && "$unresolved_resolvable_threads" -eq 0 ]]; then
    echo "Note: unresolved threads exist, but your account cannot resolve them (viewerCanResolve=false)."
  fi

  if [[ "$OUTDATED_ONLY" == "true" ]]; then
    echo "No unresolved outdated threads to resolve in $REPO PR #$PR_NUMBER."
  else
    echo "No unresolved resolvable threads to resolve in $REPO PR #$PR_NUMBER."
  fi
  exit 0
fi

echo "Target repo: $REPO"
echo "PR: #$PR_NUMBER"
echo "Threads to resolve: ${#THREAD_IDS[@]}"

if [[ "$DRY_RUN" == "true" ]]; then
  echo "Dry run enabled. Thread IDs:"
  printf '%s\n' "${THREAD_IDS[@]}"
  exit 0
fi

resolved=0
for thread_id in "${THREAD_IDS[@]}"; do
  gh api graphql -f query="$MUTATION_RESOLVE" -F id="$thread_id" >/dev/null
  resolved=$((resolved + 1))
done

echo "Resolved $resolved thread(s)."
