# advertising Specification

## Purpose
Behaviour of the `IAdvertising` client: the advertising identifier. See `common` for general rules.

## Requirements

### Requirement: Advertising id fields
`advertisingId` SHALL return `ifa`, `ifa_type` and `lmt` exactly as received, as strings, and SHALL return an error when any of the three is missing.

### Requirement: Advertising id values are checked
`advertisingId` SHALL return `InvalidParams` when `ifa_type` is not one of `dpid`, `sspid` or `sessionid`, or when `lmt` is not `0` or `1` (Firebolt 9). The client does not check that `ifa` is a UUID.

#### Scenario: Unknown ifa_type
- **WHEN** the platform answers with `ifa_type` "idfa"
- **THEN** the caller receives `InvalidParams`

#### Scenario: Unknown lmt
- **WHEN** the platform answers with `lmt` "2"
- **THEN** the caller receives `InvalidParams`

#### Scenario: Complete response
- **WHEN** the platform answers with `ifa`, `ifa_type` and `lmt`
- **THEN** the caller receives all three strings

#### Scenario: Bad response
- **WHEN** the platform answers with something that is not the expected object
- **THEN** the caller receives an error
