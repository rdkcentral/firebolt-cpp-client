## [Unreleased](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.2.1...main)

### Changed
- Added `BUILD_WITH_INSTALLED_TRANSPORT` option to allow building with the installed Transport even if the version mismatches, enabled by default

## [0.2.1](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.2.0...v0.2.1) (2025-12-17)

### Changed
- Updated Firebolt C++ Transport dependency to `v1.0.0-next.25`

## [0.2.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.1.0...v0.2.0) (2025-12-16)

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

## [0.1.0](https://github.com/rdkcentral/firebolt-cpp-client/compare/v0.0.1...v0.1.0) (2025-12-16)

### Added
- Initial Firebolt C++ Client
