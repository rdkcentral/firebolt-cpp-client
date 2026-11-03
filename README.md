# firebolt-cpp-client
Implementation of Firebolt C++ Client.

## API Test App

For usage instructions of the API test application, see:

- [test/api_test_app/README.md](test/api_test_app/README.md)

## Test Runner

Use `run-all-test.sh` to build and run unit/component tests locally.

Examples:

- `./run-all-test.sh`
- `./run-unit-tests.sh --unit-filter "ActionsUTest.*"`
- `./run-component-tests.sh --component-filter "*Localization*"`

### Testing Against Unreleased Transport Changes

For interdependent client and transport development, build the transport into a
separate install prefix and point the client build at it. This keeps the host
package untouched and opts into the development version mismatch explicitly:

```bash
TRANSPORT_DIR=/path/to/firebolt-cpp-transport
TRANSPORT_BUILD=/tmp/firebolt-transport-build
TRANSPORT_PREFIX=/tmp/firebolt-transport-install

cmake -S "$TRANSPORT_DIR" -B "$TRANSPORT_BUILD" \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_INSTALL_PREFIX="$TRANSPORT_PREFIX" \
	-DFIREBOLT_TRANSPORT_VERSION=1.2.1-dev.local
cmake --build "$TRANSPORT_BUILD" --parallel
cmake --install "$TRANSPORT_BUILD"

cmake -S . -B build-dev -DENABLE_TESTS=ON \
	-DFIREBOLT_TRANSPORT_ROOT="$TRANSPORT_PREFIX" \
	-DBUILD_WITH_INSTALLED_TRANSPORT=ON
./run-all-test.sh
```

The explicit root enables the development mismatch override, and CMake warns if
the installed package is below `.transport.version`. This affects build-time
package selection only; BitBake continues to control runtime versions.

For CI on paired pull requests, push matching branch names to both repositories
and apply the `paired-transport` label to the client PR. CI then builds the
transport branch with the same name instead of downloading a release. You can
also run `main-workflow` manually and set its `transport_ref` input to a
transport branch or tag. Without the label or input, CI uses the pinned release
archive.

To return to the pinned release dependency, unset `FIREBOLT_TRANSPORT_ROOT` and
run `./run-all-test.sh --clean` to discard the cached development package path.

For the device websocket tunnel, use `setup-device-tunnel.sh`.
Before running it, export `DEVICE_SSH_USER`, `DEVICE_SSH_HOST`, and `DEVICE_SSH_PORT`.

## Lint

Use `lint.sh` to run the same clang-format lint that CI enforces.
It checks tracked `*.cpp` and `*.h` files using:

- `git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror`

Examples:

- `./lint.sh`
- `./lint.sh --fix`
- `./lint.sh --local`
- `SKIP_DOCKER=1 ./lint.sh`

## Coverity

For Coverity build and fully unattended dependency bootstrap instructions, see:

- [COVERITY.md](COVERITY.md)
