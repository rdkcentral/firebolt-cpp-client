# advertising Specification

## Purpose
Behaviour of the `IAdvertising` client: the advertising identifier. See `common` for general rules.

## Requirements

### Requirement: Advertising id fields
`advertisingId` SHALL return `ifa`, `ifa_type` and `lmt` exactly as received, as strings, and SHALL return an error when any of the three is missing.

#### Scenario: Complete response
- **WHEN** the platform answers with `ifa`, `ifa_type` and `lmt`
- **THEN** the caller receives all three strings

#### Scenario: Bad response
- **WHEN** the platform answers with something that is not the expected object
- **THEN** the caller receives an error
