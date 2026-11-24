# discovery Specification

## Purpose
Behaviour of `Discovery.watched` that its contract does not express, including the deprecated boolean form.

## Requirements

### Requirement: watched has no result
Firebolt 9 defines `Discovery.watched` with no return value, so `watchedV2` SHALL send `Discovery.watched` and return `Result<void>`.

#### Scenario: watchedV2 call
- **WHEN** the caller calls `watchedV2` with an entity id
- **THEN** the request is `Discovery.watched` and the caller receives success with no value

### Requirement: Deprecated watched returns a boolean
`watched` is deprecated and kept for existing applications. It SHALL send the same request as `watchedV2`, with the same argument checks, and SHALL return the boolean the platform answers. It is planned to be removed, after which `watchedV2` is renamed `watched`.

#### Scenario: Platform answers true
- **WHEN** the caller calls `watched` and the platform answers true
- **THEN** the caller receives true

#### Scenario: Same checks
- **WHEN** the caller calls `watched` with an empty entity id
- **THEN** the caller receives `InvalidParams` and no request is sent

### Requirement: Progress has no upper bound
The client SHALL accept any finite `progress` of 0 or more, because it cannot tell VOD content (0 to 0.99) from live content (seconds).

#### Scenario: Live content
- **WHEN** the caller provides `progress` 3600
- **THEN** the request is sent
