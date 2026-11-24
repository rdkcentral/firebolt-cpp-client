# metrics Specification

## Purpose
Behaviour of the `IMetrics` client: sending metrics events to the platform. All calls return no value. See `common` for general rules.

## Requirements

### Requirement: Age policy is optional
Every call that accepts an age policy SHALL send `agePolicy` only when the caller provided it, as the text `app:adult`, `app:child` or `app:teen`.

#### Scenario: Age policy provided
- **WHEN** the caller calls page with "homePage" and ADULT
- **THEN** the request is `Metrics.page` with `pageId` "homePage" and `agePolicy` "app:adult"

#### Scenario: Age policy not provided
- **WHEN** the caller calls mediaPlay with an entity id and no age policy
- **THEN** the request has `entityId` and no `agePolicy`

### Requirement: Error report payload
`error` SHALL send `type` as its wire text, and `code`, `description` and `visible` always, and SHALL send `parameters` only when the caller provided them, as an object of text values.

#### Scenario: No parameters
- **WHEN** the caller calls error with type Network, a code, a description, visible true and no parameters
- **THEN** the request has `type` "network", `code`, `description` and `visible`, and no `parameters`

### Requirement: Calls without arguments
`ready`, `signIn` and `signOut` SHALL send their request without arguments.

#### Scenario: Ready
- **WHEN** the caller calls ready
- **THEN** the request is `Metrics.ready` with no params

### Requirement: Optional start and stop entity
`startContent` and `stopContent` SHALL send `entityId` only when the caller provided it.

#### Scenario: Start without entity
- **WHEN** the caller calls startContent with no entity id and no age policy
- **THEN** the request has no params

### Requirement: Event schema is a URI and data is a JSON document
`event` SHALL return `InvalidParams`, without making a platform request, when `schema` is not a URI (a scheme made of letters, digits, `+`, `-` or `.` starting with a letter, then `:`, then a non-empty remainder with no whitespace or control characters), or when `data` is not valid JSON. Firebolt 9 defines `data` as a string holding a JSON document; any valid JSON value is accepted and `data` is forwarded unchanged. The client does not check `schema`.

#### Scenario: Invalid JSON
- **WHEN** the caller calls event with `data` "foo"
- **THEN** the caller receives `InvalidParams`
- **AND** no platform request is made

#### Scenario: Valid JSON
- **WHEN** the caller calls event with `data` `{"key":"value"}`
- **THEN** the request is sent with `data` unchanged

### Requirement: Error parameters are text values
`error` SHALL accept `parameters` as a map of text keys to text values. Firebolt 9 calls them an "arg list" without further typing, so numbers and booleans are not supported.

### Requirement: Seek positions are plain numbers
`mediaSeeking` `target` and `mediaSeeked` `position` SHALL be sent as plain numbers (`double`), as in Firebolt 9. The client applies no range.

### Requirement: Error type is checked
`error` SHALL return `InvalidParams`, without making a platform request, when `type` is not one of the defined `ErrorType` values.

#### Scenario: Undefined error type
- **WHEN** the caller passes an `ErrorType` value outside the defined set
- **THEN** the caller receives `InvalidParams` and no request is sent

### Requirement: Numeric media arguments are finite
`mediaSeeking` (`target`), `mediaSeeked` (`position`) and `mediaRateChanged` (`rate`) SHALL return `InvalidParams`, without making a platform request, when the number is NaN or infinite, because such a value cannot be represented in JSON.

#### Scenario: Not a number
- **WHEN** the caller passes NaN as `target`
- **THEN** the caller receives `InvalidParams` and no request is sent
