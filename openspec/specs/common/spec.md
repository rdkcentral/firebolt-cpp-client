# common Specification

## Purpose
Rules that apply to every module of the C++ client. Module specs do not repeat them. Method shapes and wire names are in each module's `api.yaml`. A type written as `Module.Type` is defined in that module's `api.yaml` (for example `Localization.Locale` in `localization`, `Policies.AgePolicy` in `discovery`).

## Requirements

### Requirement: Calls return a Result
Every API call SHALL return a `Result` that holds either the typed value or an error. A call with no value SHALL return `Result<void>`.

#### Scenario: Successful call
- **WHEN** the platform answers a call correctly
- **THEN** the caller receives the typed value

### Requirement: Platform and transport errors reach the caller
When the underlying request fails, the client SHALL return that error to the caller and SHALL NOT return a value.

#### Scenario: Request fails
- **WHEN** the underlying request returns a general error
- **THEN** the caller receives that same error

### Requirement: Unexpected responses are errors
When a response does not have the expected shape (not the expected JSON type, a missing required field, or an unknown enum value), the client SHALL return the error `InvalidParams` and SHALL NOT return a value. This holds for every module.

#### Scenario: Missing required field
- **WHEN** a response omits a required field
- **THEN** the caller receives `InvalidParams`

#### Scenario: Unknown enum value
- **WHEN** a response contains an enum value the client does not know
- **THEN** the caller receives `InvalidParams`

### Requirement: Arguments with a fixed set of values are checked
Where the accepted values of an argument are limited to a fixed set, a fixed format or a fixed range, the client SHALL return `InvalidParams` without contacting the platform when the caller passes a value outside it. This covers enum arguments cast to an undefined value, BCP 47 language tags, numeric ranges, URIs and JSON text, as stated in each module's spec. The client does not check open-ended text (for example identifiers); the platform does. Platform error codes are never created by the client, only passed through.

#### Scenario: Value outside the accepted set
- **WHEN** the caller passes an argument outside its accepted values
- **THEN** the caller receives `InvalidParams` and no request is sent

### Requirement: Malformed events are dropped
When an event payload does not have the expected shape, the client SHALL NOT call the caller's callback, the subscription SHALL stay active, and a later well-formed event SHALL be delivered. This holds for every module that has events. The caller receives no error; the transport library logs the rejected payload.

#### Scenario: Malformed then valid event
- **WHEN** a malformed event arrives and then a well-formed one
- **THEN** the callback is called once, for the well-formed event

### Requirement: Optional parameters are sent only when provided
When an API has optional parameters, the client SHALL send an optional parameter only when the caller provided it, and SHALL send every required parameter.

#### Scenario: Optional parameter not provided
- **WHEN** the caller leaves out an optional parameter
- **THEN** the request does not contain that key

### Requirement: Event subscriptions
Each `subscribeOn...` function SHALL return a `SubscriptionId` on success and SHALL deliver each event's typed payload to the caller's callback. `unsubscribe(id)` SHALL remove one subscription and `unsubscribeAll()` SHALL remove every subscription of that module. A subscribe error SHALL be returned to the caller.

#### Scenario: Subscribe and unsubscribe
- **WHEN** the caller subscribes, then calls unsubscribe with the returned id
- **THEN** the subscription is removed and the call reports success

#### Scenario: Subscribe fails
- **WHEN** the underlying subscribe returns a general error
- **THEN** the caller receives that error and no `SubscriptionId`
