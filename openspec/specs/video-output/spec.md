# video-output Specification

## Purpose
Behaviour of the `IVideoOutput` client: properties of the active video output and change events for four of them. See `common` for general rules.

## Requirements

### Requirement: Unknown values are InvalidParams
For `hdcp`, `cecState`, `colorFormat`, `colorimetry`, `dynamicRange`, `quantizationRange`, `refreshRate` and `colorDepth`, the client SHALL return the error `InvalidParams` when the platform answers with a value that is not in the known set.

#### Scenario: Unknown HDCP value
- **WHEN** the platform answers `VideoOutput.hdcp` with a value that is not a known state
- **THEN** the caller receives `InvalidParams`

### Requirement: Malformed resolution is InvalidParams
`resolution` SHALL return `InvalidParams` when the payload is malformed, for example when `height` is missing.

#### Scenario: Missing height
- **WHEN** the platform answers `VideoOutput.resolution` with only `width`
- **THEN** the caller receives `InvalidParams`

### Requirement: Numeric properties
`refreshRate` and `colorDepth` SHALL accept the platform's value as a number (for example 59.94, or a whole number written as a float such as 24.0 for `refreshRate` or 10.0 for `colorDepth`), as Firebolt 9 defines them and as the platform sends them. They SHALL also accept the same value as text (for example "59.94") for compatibility with a platform that serialises them as text; this tolerance is deliberate. In every case the value SHALL be in the Firebolt 9 set, otherwise the client SHALL return `InvalidParams`; a large integer SHALL NOT wrap into a valid value (for example 4294967320 is not 24). `resolution` `width` and `height` are `unsigned` values (see `common`).

#### Scenario: Refresh rate as text
- **WHEN** the platform answers `VideoOutput.refreshRate` with "59.94"
- **THEN** the caller receives the 59.94 Hz value

#### Scenario: Refresh rate as number
- **WHEN** the platform answers `VideoOutput.refreshRate` with the number 59.94
- **THEN** the caller receives the 59.94 Hz value

#### Scenario: Unsupported number
- **WHEN** the platform answers `VideoOutput.refreshRate` with 59 or 61
- **THEN** the caller receives `InvalidParams`

### Requirement: Events deliver parsed values
A change subscription SHALL deliver the parsed value to the callback. When an event payload is malformed, the client SHALL NOT call the callback and the subscription SHALL stay active. The caller receives no error; the transport library logs the rejected event at error level (event name and payload), so the client adds no logging of its own.

#### Scenario: Resolution event
- **WHEN** the platform sends a resolution event with height 2160 and width 3840
- **THEN** the callback receives height 2160 and width 3840

#### Scenario: Malformed event
- **WHEN** the platform sends a resolution event without `height`
- **THEN** the callback is not called

### Requirement: Unsubscribe forwarding
`unsubscribe` and `unsubscribeAll` SHALL be forwarded to the transport, and an error from the transport SHALL be returned to the caller.

#### Scenario: Unsubscribe fails
- **WHEN** the transport returns an error for unsubscribe
- **THEN** the caller receives that error
