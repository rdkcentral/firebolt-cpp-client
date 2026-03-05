# API Test App

`api-test-app` is a CLI demo/test utility for exercising Firebolt C++ client APIs.
It can connect to a local mock service, a platform service, or any endpoint set through an environment variable.
It can be run in either interactive or auto mode.
In interactive mode, the user can select which APIs to call via a menu system.
In auto mode, every API method is called sequentially, with no input from the user.

All output is to stdout.

## Executable Name

The app binary name is:

`api-test-app`

## Command Line Usage

```bash
api-test-app [--auto] [--mock] [--platform] [--url <URL>] [--dbg] [--help]
```

### Options

- `--auto`
 Run all methods for all interfaces automatically.

- `--mock`
 Use local mock endpoint: `ws://127.0.0.1:9998/`.

- `--platform`
 Use platform endpoint: `ws://127.0.0.1:3474/`.

- `--url URL`
 Set the WebSocket endpoint to the specified URL.

- `--dbg`
 Enable debug logging.

- `--help`
 Print usage and exit.

## Endpoint Selection Priority

The app chooses the WebSocket endpoint in this order:

1. `--mock` or `--platform` or `--url URL` (if provided)
2. `FIREBOLT_ENDPOINT` environment variable (if set)
3. Default fallback: `ws://127.0.0.1:9998/`

At startup, the app prints the selected URL:

```text
Using firebolt URL: <url>
```

## Run Modes

### 1) Interactive mode (default when stdin is a TTY)

- Shows a menu of interfaces.
- After selecting an interface, shows its methods.
- Runs selected methods on demand.

### 2) Auto mode (`--auto`)

- Runs every method of every interface sequentially.
- Prints each interface and method before execution.

### 3) Piped stdin mode (non-TTY stdin)

When input is piped in, the app automatically switches to autorun behavior for command processing and reads one method name per line.

Each line must match a method string exactly (for example, `Accessibility.highContrastUI`).
If a method is unknown, it prints:

```text
Method not found: <input>
```

## Examples

### Use mock service (default behavior in helper script)

```bash
api-test-app --mock
```

### Connect to platform service with debug logs

```bash
api-test-app --platform --dbg
```

### Use custom endpoint via environment

```bash
export FIREBOLT_ENDPOINT="ws://192.168.1.50:3474/"
api-test-app
```

### Run all methods automatically

```bash
api-test-app --auto --mock
```

### Run methods from a test suite file

```bash
cat test-suite.example | api-test-app --mock
```

## Connection Behavior

- The app attempts to connect and waits up to 2 seconds for initial connection.
- It exits with non-zero status if connection setup fails or times out.
- On completion, it disconnects cleanly.

## Build Script Notes

The local helper script `build.sh` in this directory:

- Builds the app in `build/`
- Runs `build/api-test-app --mock` by default
- Supports options such as `--clean`, `--no-run`, `--just-run`, and `--sysroot`
