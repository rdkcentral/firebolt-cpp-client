# device Specification

## Purpose
Behaviour of the `Device` module that its contract does not express.

## Requirements

### Requirement: OS setters are testing extensions
`setOsName` and `setOsVersion` are not defined by Firebolt 9, which has only the `osName` and `osVersion` getters. They SHALL send the caller's text as the `value` param, applications SHALL NOT rely on them, and their header comments SHALL say so.

#### Scenario: Set the OS name
- **WHEN** the caller calls `setOsName` with "Linux"
- **THEN** the request is `Device.setOsName` with `value` "Linux"
