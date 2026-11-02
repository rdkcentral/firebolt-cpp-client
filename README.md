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
