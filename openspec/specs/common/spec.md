# common Specification

## Purpose
Rules that hold for every module of the C++ client. Module specs do not repeat them. The words `required`, `optional`, `values`, `minimum`, `maximum`, `format` and `unsigned` refer to the module's `api.yaml` (see `openspec/README.md`).

## Requirements

### Requirement: Module access
The client SHALL provide each module through `IFireboltAccessor::<Module>Interface()`, and `Disconnect()` SHALL remove the subscriptions of every module that has events.

#### Scenario: Interface is available
- **WHEN** the caller asks the accessor for `DeviceInterface()`
- **THEN** it receives the `IDevice` interface

#### Scenario: Disconnect
- **WHEN** the caller has subscribed to events and calls `Disconnect()`
- **THEN** the subscriptions are removed

### Requirement: Calls return a Result
Every call SHALL return a `Result` that holds either the typed value or an error, and a call with no result SHALL return `Result<void>`.

#### Scenario: Successful call
- **WHEN** the platform answers a call correctly
- **THEN** the caller receives the typed value

### Requirement: Platform errors reach the caller
When the request fails, the client SHALL return that error unchanged, including the specific codes in `common/api.yaml`, and SHALL NOT return a value. The client never creates a platform error code.

#### Scenario: Request fails
- **WHEN** the platform answers `Device.timeInActiveState` with error -50110
- **THEN** the caller receives the error -50110

### Requirement: Arguments are checked before sending
The client SHALL return `InvalidParams` without sending a request when an argument breaks a limit declared for it (`values`, `minimum`, `maximum`, `minLength`, `pattern`, `format`) or is a `double` that is NaN or infinite. Text without a declared limit is not checked; the platform checks it. Enum arguments are checked only where a module spec says so.

#### Scenario: Argument outside its limit
- **WHEN** the caller passes `SpeechSynthesis.speak` a `volume` of 1.1
- **THEN** the caller receives `InvalidParams` and no request is sent

#### Scenario: Limits are inclusive
- **WHEN** the caller passes `volume` 0, `rate` 0.1 and `pitch` 2
- **THEN** the request is sent

### Requirement: Optional arguments are sent only when provided
The client SHALL send every required param, and SHALL send an optional param only when the caller provided it. An empty string is a provided value.

#### Scenario: Optional argument left out
- **WHEN** the caller calls `Metrics.page` without an age policy
- **THEN** the request contains `pageId` and no `agePolicy`

#### Scenario: Empty string provided
- **WHEN** the caller passes an empty `voice` to `TextToSpeech.speak`
- **THEN** the request contains `voice` with the empty string

### Requirement: Responses are checked
The client SHALL return `InvalidParams`, and no value, when a response does not match its type: the JSON type, a missing required field, an enumerator or `values` entry it does not know, or an `unsigned` that is not an integer from 0 to 4294967295 (`unsigned64`: 0 to 18446744073709551615). A negative number, a fraction (even `2.0`), a boolean or text is not an unsigned integer, and a larger value does not wrap.

#### Scenario: Missing field
- **WHEN** the platform answers `Display.size` without `height`
- **THEN** the caller receives `InvalidParams`

#### Scenario: Unsigned out of range
- **WHEN** the platform answers `Device.uptime` with -1 or 4294967296
- **THEN** the caller receives `InvalidParams`

#### Scenario: Unsigned64 above 4 GiB
- **WHEN** the platform answers `Stats.memoryUsage` with `userMemoryUsed` 8589934592
- **THEN** the caller receives 8589934592

### Requirement: Platform-owned limits are not checked on responses
The client SHALL return a response value that has the right type but breaks a `minimum`, `maximum`, `minLength`, `pattern` or `format`, because those describe what the platform produces.

#### Scenario: Value outside the range
- **WHEN** the platform answers `Accessibility.voiceGuidanceSettings` with a `rate` of 11
- **THEN** the caller receives the settings with `rate` 11

### Requirement: Malformed events are dropped
When an event payload does not match its type, the client SHALL NOT call the callback, the subscription SHALL stay active, and a later well-formed event SHALL be delivered. The caller receives no error; the transport library logs the rejected payload.

#### Scenario: Malformed then valid event
- **WHEN** a malformed event arrives and then a well-formed one
- **THEN** the callback is called once, for the well-formed event

### Requirement: Event subscriptions
`subscribeOn<Event>` SHALL return a `SubscriptionId` and SHALL deliver each event's typed payload to the callback. `unsubscribe(id)` SHALL remove one subscription and `unsubscribeAll()` SHALL remove all of the module's. A failing subscribe or unsubscribe SHALL return its error.

#### Scenario: Subscribe and unsubscribe
- **WHEN** the caller subscribes and then calls `unsubscribe` with the returned id
- **THEN** the callback is no longer called and the call reports success

#### Scenario: Subscribe fails
- **WHEN** the transport returns an error for the subscribe
- **THEN** the caller receives that error and no `SubscriptionId`

### Requirement: Format bcp47
An argument with `format: bcp47` SHALL be a syntactically well-formed BCP 47 language tag (RFC 5646 syntax; the client does not check that subtags are registered).

#### Scenario: Well-formed tag
- **WHEN** the caller passes "en", "en-US", "zh-Hant-TW" or "EN-us"
- **THEN** the argument is accepted

#### Scenario: Malformed tag
- **WHEN** the caller passes "", "e", "en_US" or "en--US"
- **THEN** the caller receives `InvalidParams`

### Requirement: Format timestamp
An argument with `format: timestamp` SHALL be exactly `YYYY-MM-DDThh:mm:ss.sssZ`: UTC with the `Z` suffix and three fraction digits, with month 1 to 12, day 1 to 31, hour 0 to 23, minute and second 0 to 59. The platform checks the calendar (days per month, leap years).

#### Scenario: Valid timestamp
- **WHEN** the caller passes "2021-04-23T18:25:43.511Z"
- **THEN** the argument is accepted

#### Scenario: Malformed timestamp
- **WHEN** the caller passes "2024-13-01T00:00:00.000Z", "2024-06-01T12:00:00Z" or "2024-06-01T12:00:00.000+00:00"
- **THEN** the caller receives `InvalidParams`

### Requirement: Format uri
An argument with `format: uri` SHALL be a scheme (a letter, then letters, digits, `+`, `-` or `.`), then `:`, then a non-empty remainder without whitespace or control characters.

#### Scenario: Valid URI
- **WHEN** the caller passes "http://meta.rdkcentral.com/some/schema"
- **THEN** the argument is accepted

#### Scenario: Malformed URI
- **WHEN** the caller passes "no-scheme" or "http: x"
- **THEN** the caller receives `InvalidParams`

### Requirement: Format json
An argument of type `json`, or with `format: json`, SHALL be text that is valid JSON of any kind. `Metrics.event` sends it unchanged as text; `Actions.start` sends the parsed value (see `actions`).

#### Scenario: Valid JSON text
- **WHEN** the caller passes `{"key":"value"}`
- **THEN** the argument is accepted

#### Scenario: Invalid JSON text
- **WHEN** the caller passes `foo`
- **THEN** the caller receives `InvalidParams`

### Requirement: Format decimal
An argument with `format: decimal` SHALL be a plain unsigned decimal number: digits with at most one `.`, and no sign, exponent, space, comma, `nan` or `inf`. The `minimum` and `maximum` apply to its numeric value.

#### Scenario: Valid decimal
- **WHEN** the caller passes "0", "0.1" or "1.25"
- **THEN** the argument is accepted

#### Scenario: Invalid decimal
- **WHEN** the caller passes "", "fast", "-1", "1e1" or "1,5"
- **THEN** the caller receives `InvalidParams`
