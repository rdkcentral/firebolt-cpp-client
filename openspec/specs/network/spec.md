# network Specification

## Purpose
Behaviour of the `INetwork` client: network connectivity. See `common` for general rules.

## Requirements

### Requirement: Connectivity is a boolean
`connected` SHALL return the platform's boolean.

#### Scenario: Connected
- **WHEN** the platform answers `Network.connected` with true
- **THEN** the caller receives true
