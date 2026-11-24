# video-output Specification

## Purpose
Behaviour of the `VideoOutput` module that its contract does not express.

## Requirements

### Requirement: Numbers are also accepted as text
`refreshRate` and `colorDepth` SHALL accept the platform's value as a number, including a whole number written as a float such as 24.0 or 10.0, and, deliberately, as the same value in text, such as "59.94", for platforms that send text. The value SHALL be in the enum's set, otherwise the client SHALL return `InvalidParams`, and a large integer SHALL NOT wrap into a valid value.

#### Scenario: Refresh rate as text
- **WHEN** the platform answers `VideoOutput.refreshRate` with "59.94"
- **THEN** the caller receives the 59.94 Hz value

#### Scenario: Refresh rate as number
- **WHEN** the platform answers `VideoOutput.refreshRate` with the number 59.94
- **THEN** the caller receives the 59.94 Hz value

#### Scenario: Value not in the set
- **WHEN** the platform answers `VideoOutput.refreshRate` with 59, 61 or 4294967320 (24 modulo 2^32), or `VideoOutput.colorDepth` with 4294967306 (10 modulo 2^32)
- **THEN** the caller receives `InvalidParams`
