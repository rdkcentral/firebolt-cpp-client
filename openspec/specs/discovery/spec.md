# discovery Specification

## Purpose
Behaviour of the `IDiscovery` client: telling the platform that content was watched. See `common` for general rules.

## Requirements

### Requirement: Watched payload
`watched` SHALL send `entityId` always, and SHALL send `progress`, `completed`, `watchedOn` and `agePolicy` only when the caller provided them. `agePolicy` SHALL be sent as the text `app:adult`, `app:child` or `app:teen`.

#### Scenario: All arguments provided
- **WHEN** the caller provides entityId, progress 0.75, completed true, watchedOn "2024-06-01T12:00:00Z" and agePolicy ADULT
- **THEN** the request is `Discovery.watched` with exactly those five params and `agePolicy` "app:adult"

### Requirement: Watched result
`watched` SHALL return the boolean answered by the platform. Note: Firebolt 9 defines `Discovery.watched` with no return value; this requirement records the current client behaviour and is tracked as DI-1 in `openspec/OPEN_QUESTIONS.md`.

#### Scenario: Platform answers true
- **WHEN** the platform answers true
- **THEN** the caller receives true

### Requirement: watchedV2 uses the watched wire method
`watchedV2` SHALL send the same payload as `watched`, using the wire method `Discovery.watched`, and SHALL return no value.

#### Scenario: watchedV2 call
- **WHEN** the caller calls watchedV2
- **THEN** the request is `Discovery.watched` with the same params as for `watched`
