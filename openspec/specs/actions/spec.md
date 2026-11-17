# actions Specification

## Purpose
Behaviour of the `IActions` client: receiving and sending intents. See `common` for general rules.

## Requirements

### Requirement: Intent text is parsed before sending
`start` SHALL parse its `intent` argument as JSON and send the parsed value as the `intent` param, and SHALL send `handlerAppId` only when the caller provided it.

#### Scenario: Valid intent
- **WHEN** the caller calls start with the text `{"action":"launch"}`
- **THEN** the request is `Actions.start` with `intent` set to the JSON object

#### Scenario: Not an object
- **WHEN** the caller calls start with JSON text that is not an object, such as an array or string
- **THEN** the parsed value is sent unchanged

### Requirement: Malformed intent is rejected locally
`start` SHALL return the error `InvalidParams` without sending a request when the `intent` text is not valid JSON, and SHALL NOT throw.

#### Scenario: Malformed JSON
- **WHEN** the caller calls start with text that is not valid JSON
- **THEN** the caller receives `InvalidParams` and no request is sent

### Requirement: Received intent is kept as JSON text
`intent` and the `onIntent` event SHALL return the intent payload as JSON text together with `intentId`, and SHALL keep payloads that are not objects.

#### Scenario: Object payload
- **WHEN** the platform answers `Actions.intent` with an object payload
- **THEN** the caller receives that payload serialized as JSON text

#### Scenario: Non-object payload
- **WHEN** the platform answers with an array or string payload
- **THEN** the caller receives it serialized as JSON text, unchanged
