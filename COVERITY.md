# Coverity Build Guide

This document describes the Coverity-friendly build flow for firebolt-cpp-client, including fully unattended transport dependency provisioning.

## Purpose

`cov_build.sh` is designed to run in clean/off nodes where FireboltTransport may not already be installed.

The script configures and builds a Debug test-enabled build so Coverity can capture both library and test compilation units.

## Quick Start

Run from repo root:

```bash
./cov_build.sh
```

Strict no-host-dependency mode:

```bash
COV_FORCE_BOOTSTRAP_TRANSPORT=1 \
COV_SKIP_SYSTEM_TRANSPORT=1 \
COV_FORCE_RELEASE_TRANSPORT=1 \
./cov_build.sh
```

Local ad-hoc workflow test via act:

```bash
make -f Makefile.act act-native
```

Faster local loop when dependencies are already present in the container image:

```bash
make -f Makefile.act act-native-fast
```

## Dependency Resolution Order

`cov_build.sh` resolves FireboltTransport in this order.

1. Explicit `FireboltTransport_DIR` if provided.
2. Local bootstrap prefix (`COV_DEPS_PREFIX`, default `.cov-deps`).
3. System CMake package paths (`/usr/local/...`, `/usr/...`) unless disabled.
4. Sibling repo bootstrap from `../firebolt-cpp-transport`.
5. Release tarball bootstrap using pinned version from `.transport.version`.

After bootstrap, the script passes `-DFireboltTransport_DIR=<resolved-config-dir>` to CMake for deterministic package selection.

## Hands-Off Bootstrap Paths

### Sibling repo bootstrap

If `../firebolt-cpp-transport` exists, the script builds and installs it into `COV_DEPS_PREFIX`.

### Release tarball bootstrap

If sibling repo is unavailable or `COV_FORCE_RELEASE_TRANSPORT=1` is set:

1. Read pinned version from `.transport.version`.
2. Download release archive from GitHub releases.
3. Extract source into `.cov-deps/src/`.
4. Build/install into `COV_DEPS_PREFIX`.
5. Re-run client configure/build against the installed package config.

The transport bootstrap passes:

```bash
-DFIREBOLT_TRANSPORT_VERSION=<value from .transport.version>
```

This keeps package version compatibility aligned with client `find_package(FireboltTransport <major> CONFIG REQUIRED)`.

## Environment Variables

### `COV_DEPS_PREFIX`

Install/bootstrap prefix for local dependencies.

Default:

```bash
<repo>/.cov-deps
```

### `COV_SKIP_SYSTEM_TRANSPORT`

When set to `1`, skip system package search paths for FireboltTransport.

Use this to guarantee host-independent behavior.

### `COV_FORCE_BOOTSTRAP_TRANSPORT`

When set to `1`, do not reuse an already-discovered transport package. Force bootstrap flow.

### `COV_FORCE_RELEASE_TRANSPORT`

When set to `1`, bypass sibling repo bootstrap and force release tarball bootstrap.

## CMake Root Hinting

In addition to script bootstrap, CMake supports explicit root hinting:

- `FIREBOLT_TRANSPORT_ROOT` (project-level convenience variable)
- `FireboltTransport_ROOT` (package-native CMake variable)

Either can point to an install prefix containing `lib/cmake/FireboltTransport`.

## CI Recommendations

Use strict mode for reproducibility:

```bash
COV_FORCE_BOOTSTRAP_TRANSPORT=1 \
COV_SKIP_SYSTEM_TRANSPORT=1 \
COV_FORCE_RELEASE_TRANSPORT=1 \
./cov_build.sh
```

This prevents accidental coupling to preinstalled host packages.

## Troubleshooting

### Missing `.transport.version`

Symptom: bootstrap fails before download.

Fix: ensure `.transport.version` exists and contains a non-empty version.

### Release download failure

Symptom: both release URL patterns fail.

Fix: verify pinned version exists in `rdkcentral/firebolt-cpp-transport` releases and that node has outbound network access.

### Version mismatch shown during configure

Symptom: `installed version` differs from `expected`.

Fix: use strict mode and a clean `COV_DEPS_PREFIX`, or set `COV_FORCE_RELEASE_TRANSPORT=1` to rebuild from pinned release.

## Artifacts

When release bootstrap is used, expected local artifacts include:

- `.cov-deps/src/firebolt-cpp-transport-<version>/`
- `.cov-deps/lib/cmake/FireboltTransport/FireboltTransportConfig.cmake`
- `.cov-deps/lib/libFireboltTransport.so*`
