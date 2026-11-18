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
`watched` SHALL return the boolean answered by the platform. `watched` is deprecated and kept only for existing applications: Firebolt 9 defines `Discovery.watched` with no return value, which is what `watchedV2` implements. `watchedV2` is planned to be renamed `watched`, and the boolean-returning method removed (DI-1 in `openspec/OPEN_QUESTIONS.md`).

#### Scenario: Platform answers true
- **WHEN** the platform answers true
- **THEN** the caller receives true

### Requirement: Entity id and timestamp are checked
`watched` and `watchedV2` SHALL return `InvalidParams`, without making a platform request, when `entityId` is empty, or when `watchedOn` is provided and is not a valid UTC timestamp. Firebolt 9 defines `watchedOn` as `YYYY-MM-DDThh:mm:ss.sssZ`; the client also accepts whole seconds and other fraction lengths (for example `2024-06-01T12:00:00Z`) so existing callers keep working. The date and time SHALL be real values (month 1 to 12, a day that exists in that month including leap years, hour 0 to 23, minute and second 0 to 59). Time zone offsets such as `+01:00` are not accepted.

#### Scenario: Empty entity id
- **WHEN** the caller provides an empty `entityId`
- **THEN** the caller receives `InvalidParams`
- **AND** no platform request is made

#### Scenario: Malformed timestamp
- **WHEN** the caller provides `watchedOn` "2023-02-29T00:00:00Z" or "2024-06-01T12:00:00+00:00"
- **THEN** the caller receives `InvalidParams`
- **AND** no platform request is made

#### Scenario: Valid timestamps
- **WHEN** the caller provides `watchedOn` "2021-04-23T18:25:43.511Z" or "2024-06-01T12:00:00Z"
- **THEN** the request is sent

### Requirement: Progress meaning
`progress` is 0 to 0.99 for VOD content and a number of seconds for live content (Firebolt 9). Because the client cannot tell VOD from live, it SHALL only reject a `progress` that is negative, NaN or infinite, returning `InvalidParams` from `watched` and `watchedV2` without making a platform request.

#### Scenario: Negative progress
- **WHEN** the caller provides progress -0.1
- **THEN** the caller receives `InvalidParams`
- **AND** no platform request is made

#### Scenario: Large progress for live content
- **WHEN** the caller provides progress 3600
- **THEN** the request is sent

### Requirement: watchedV2 uses the watched wire method
`watchedV2` SHALL send the same payload as `watched`, using the wire method `Discovery.watched`, and SHALL return no value.

#### Scenario: watchedV2 call
- **WHEN** the caller calls watchedV2
- **THEN** the request is `Discovery.watched` with the same params as for `watched`
