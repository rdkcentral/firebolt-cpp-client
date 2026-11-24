# display Specification

## Purpose
Behaviour of the `Display` module that its contract does not express.

## Requirements

### Requirement: No display is a successful result
When an OTT or STB device has no display connected over HDMI, the client SHALL return the platform's `""` for `edid` and `0, 0` for `size` and `maxResolution` as successful results, not as errors.

#### Scenario: Empty EDID
- **WHEN** the platform answers `Display.edid` with ""
- **THEN** the caller receives the empty string as a successful result

#### Scenario: Zero size
- **WHEN** the platform answers `Display.size` with width 0 and height 0
- **THEN** the caller receives width 0 and height 0 as a successful result
