## [0.7.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.6.4...v0.7.0)

### Added
- New APIs
  - `Localization.timeZone` getter and `onTimeZoneChanged` event
  - `Device.dolbyAtmosExperienceAvailable` getter and `onDolbyAtmosExperienceAvailableChanged` event
  - `Device.osName`, `Device.osVersion`, and `Device.firmware`
  - `SpeechSynthesis` (`speak`, `cancel`, `pause`, `resume`, `voices`, `subscribeVoiceChanged`, `onUtteranceEvent`)
  - `VideoOutput` implementation

### Changed
- **Breaking**: `Stats.memoryUsage` now returns the value in bytes 
- **Breaking**: `Discovery.watchedV2` return type changed to `Result<void>`

## [0.6.4](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.6.3...v0.6.4)

### Changed
- **Breaking**: `Actions.intent()` return type changed from `Result<std::string>` to `Result<Intent>`
- **Breaking**: `subscribeOnIntent` / `subscribeOnIntentChanged` callback parameter changed from `const std::string&` to `const Intent&`

### Added
- `Actions.start(const IntentData& intent, std::optional<std::string> handlerAppId)` — new API per Firebolt 9 spec
- New public types: `Firebolt::Actions::Intent`, `IntentData`, `IntentContext`

## [0.6.3](https://github.com/rdkcentral/firebolt-cpp-client/compare/0.6.2...v0.6.3)

### Fixed
- `Actions.intent` and `Actions.onIntent` now correctly handle a JSON object payload (`{"intent":"...","intentId":N}`) sent by the Firebolt backend. Previously the client failed to parse the response because it expected a plain string.

## [0.6.2](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.6.1...0.6.2)

### Fixed
- `Discovery.watched` now returns `Result<bool>` again (reverts the `Result<void>` change introduced in v0.6.0).

## [0.6.1](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.6.0...v0.6.1)

### Added
- `Discovery.watchedV2`: same as `Discovery.watched` but returns `Result<bool>` - compatibility shim for callers migrating away from the pre-v0.6.0 boolean return type

## [0.6.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.5.5...v0.6.0)

### Added
- New APIs
  - `Actions.intent` (no parameters, returns string)
  - `Actions.onIntent` event

### Changed
- **Breaking**: `Discovery.watched` and all `Metrics.*` methods now return `Result<void>` (previously `Result<bool>`)

## [0.5.5](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.5.4...v0.5.5)

### Changed
- Allow building without a SONAME

## [0.5.4](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.5.3...v0.5.4)

### Fixed
- Enums for Lifecycle.CloseType were not aligned with the spec: `killReload`, `killReactivate`

## [0.5.3](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.5.2...v0.5.3)

### Changed
- Added protocol selection to the API Test App: `[--legacy | --rpc-v2]`
- Updated Firebolt C++ Transport dependency to `v1.1.5`: in legacy protocol result in event payload
  can be an array

## [0.5.2](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.5.1...v0.5.2)

### Changed
- **Breaking**: Removed `FetchContent` from the build system; the Firebolt C++ Transport must now
  be installed separately (along with other dependencies) before building the project

## [0.5.1](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.5.0...v0.5.1)

### Changed
- Updated Firebolt C++ Transport dependency to `v1.1.3`
- In API Test App, URL can be specified on command line: `--url URL`

### Fixed
- Prevented a crash when a mandatory field was missing from the JSON payload

## [0.5.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.4.0...v0.5.0)

### Added
- New APIs
  - Device.hdr
  - Display.edid
  - Metrics
  - Network
  - TextToSpeech

### Changed
- **Breaking**: Aligned types with the latest specification updates:
  - `integer` changed to unsigned (`uint32_t`)
  - `float` changed to `double`
- **Breaking**: `AgePolicy` was moved to the `Firebolt` namespace and to the `common_types.h` header file. References must be changed
  from `Firebolt::Discovery::AgePolicy` to `Firebolt::AgePolicy`, and the include must be added (`#include <firebolt/common_types.h>`).
- Specify the build version and ABI version
- Updated Firebolt C++ Transport dependency to `v1.1.2`

## [0.4.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.3.0...v0.4.0)

### Changed
- Updated Firebolt C++ Transport dependency to `v1.1.0`, which allows use of the legacy RPC-v1 protocol

## [0.3.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.2.0...v0.3)

### Added
- New APIs
  - Discovery

### Changed
- Updated Firebolt C++ Transport dependency to `v1.0.0`
- Added `BUILD_WITH_INSTALLED_TRANSPORT` option to allow building with the installed Transport even if the version mismatches, enabled by default

## [0.2.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.1.0...v0.2.0)

### Added
- New APIs
  - Accessibility
  - Advertising
  - Device
  - Display
  - Lifecycle 2.0
  - Localization
  - Presentation
  - Stats

### Changed
- **Breaking**: Header files have been moved to a `firebolt/` subdirectory. Include paths must be updated (e.g., `#include <firebolt/firebolt.h>`)
- **Breaking**: The client initialization and deinitialization flow has been updated
- The dependency on the Thunder framework has been removed
- Updated Firebolt C++ Transport dependency to `v1.0.0-next.23`

### Removed
- The following APIs, which were not part of the formal specification, have been removed:
  - ClosedCaptions
  - HdmiInput
  - Lifecycle 1.0
  - Metrics
  - SecureStorage

## [0.1.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.0.1...v0.1.0)

### Added
- Initial Firebolt C++ Client
