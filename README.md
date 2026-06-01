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

Use `lint.sh` to run local static analysis for C/C++ sources.

Examples:

- `./lint.sh`
- `./lint.sh --tidy-only`
- `./lint.sh --tidy-only --fix`
- `./lint.sh --cppcheck-only`

## Client Versioning

`Firebolt.clientVersion` is resolved at configure/build time with this precedence:

1. `FIREBOLT_CLIENT_VERSION` environment variable
2. `.client-version` file in the repository/source root
3. current git short SHA
4. `unknown`

For source-distribution releases (where git metadata may be unavailable), bake the release version into `.client-version` before creating the tarball.
