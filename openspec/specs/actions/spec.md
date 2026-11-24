# actions Specification

## Purpose
How the client turns an intent between its C++ form (JSON text) and its wire form (a JSON value) when receiving and sending intents.

## Requirements

### Requirement: Sent intent is parsed
`start` SHALL parse its `intent` text and send the parsed JSON value, of any kind, as the `intent` param. When the text is not valid JSON, `start` SHALL return `InvalidParams` without sending a request and without throwing.

#### Scenario: Valid intent
- **WHEN** the caller calls `start` with the text `{"action":"launch"}`
- **THEN** the request is `Actions.start` with `intent` set to the JSON object

#### Scenario: Intent that is not an object
- **WHEN** the caller calls `start` with the text `[1,2]` or `"text"`
- **THEN** the request carries that array or string unchanged

#### Scenario: Malformed intent
- **WHEN** the caller calls `start` with text that is not valid JSON
- **THEN** the caller receives `InvalidParams` and no request is sent

### Requirement: Received intent is returned as text
`intent` and the `onIntent` event SHALL return the received `intent` value serialized as JSON text, together with `intentId`, whatever kind of JSON value it is.

#### Scenario: Object intent
- **WHEN** the platform answers `Actions.intent` with an object
- **THEN** the caller receives that object serialized as text

#### Scenario: Intent that is not an object
- **WHEN** the platform answers with an array or a string
- **THEN** the caller receives it serialized as text, unchanged
