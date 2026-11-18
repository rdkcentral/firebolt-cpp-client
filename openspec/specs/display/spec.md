# display Specification

## Purpose
Behaviour of the `IDisplay` client: details of the connected or integral display. See `common` for general rules.

## Requirements

### Requirement: Display values returned as received
`edid`, `size` and `maxResolution` SHALL return the platform's values unchanged, including `0, 0` for `size` and `maxResolution` when an OTT or STB device has no display connected over HDMI (as stated in Firebolt 9).

#### Scenario: Empty EDID without a display
- **WHEN** an OTT or STB device has no display connected over HDMI and the platform answers `Display.edid` with ""
- **THEN** the caller receives the empty string as a successful result, not an error

#### Scenario: Maximum resolution
- **WHEN** the platform answers `Display.maxResolution` with width 1920 and height 1080
- **THEN** the caller receives width 1920 and height 1080

### Requirement: Size objects need both dimensions
`size` and `maxResolution` SHALL return an error when `width` or `height` is missing.

#### Scenario: Missing height
- **WHEN** the platform answers `Display.size` without `height`
- **THEN** the caller receives an error
