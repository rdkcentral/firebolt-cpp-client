# device Specification

## Purpose
Behaviour of the `IDevice` client: device information, two setters and change events. See `common` for general rules.

## Requirements

### Requirement: Setters are testing extensions
`setOsName` and `setOsVersion` are not defined by Firebolt 9 (which has only the `osName` and `osVersion` getters). They are client extensions for testing only; applications SHALL NOT rely on them, and the header SHALL say so. They are non-`const` because they change platform state, unlike the read-only getters.

### Requirement: Setters send the value param
`setOsName` and `setOsVersion` SHALL send the caller's text as the `value` param.

#### Scenario: Set OS name
- **WHEN** the caller calls setOsName with "Linux"
- **THEN** the request is `Device.setOsName` with `value` "Linux"

### Requirement: Device class must be known
`deviceClass` SHALL return an error when the platform answers with a class other than `stb`, `ott` or `tv`.

#### Scenario: Unknown class
- **WHEN** the platform answers with an unknown device class
- **THEN** the caller receives an error

### Requirement: HDR formats need every flag
`hdr` SHALL require `hdr10`, `hdr10Plus`, `dolbyVision` and `hlg` in the payload.

#### Scenario: Missing flag
- **WHEN** the platform sends HDR formats without `hlg`
- **THEN** the caller receives an error

### Requirement: Platform errors pass through
`timeInActiveState` SHALL return any platform error unchanged, including the Firebolt 9 specific error -50110 (app state invalid). The client does not itself decide whether the app state is valid.

#### Scenario: App state invalid
- **WHEN** the platform answers `Device.timeInActiveState` with error -50110
- **THEN** the caller receives that same error code
