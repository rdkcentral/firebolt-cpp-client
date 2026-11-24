# text-to-speech Specification

## Purpose
Behaviour of the C++ TextToSpeech client (`ITextToSpeech`). Method names, types and wire names are in `api.yaml`; the `speak` parameters are also listed below because their accepted values matter. Methods are referred to by their wire name, for example `TextToSpeech.speak`.

## Requirements

### Requirement: Module access
The client SHALL expose TextToSpeech through `IFireboltAccessor::TextToSpeechInterface()`, and SHALL remove all TextToSpeech subscriptions when the accessor removes all subscriptions.

#### Scenario: Interface is available
- **WHEN** the caller requests `TextToSpeechInterface()` from the accessor
- **THEN** it receives the `ITextToSpeech` interface

### Requirement: Speak parameters
`TextToSpeech.speak` SHALL accept the following parameters. Only `text` is defined by Firebolt 9; the rest are defined by the Text To Speech Plugin contract.

| Parameter | Required | Meaning | Accepted values |
|---|---|---|---|
| `text` | yes | Utterance | Plain text or a well-formed SSML document (not checked by the client) |
| `callsign` | no | Application call sign | Any string; the platform requires it when `setACL` was called earlier |
| `language` | no | Language of the text | BCP 47 tag |
| `voice` | no | Voice name | A name returned by `listvoices` (not checked by the client) |
| `volume` | no | Speaking volume | Decimal string, 0 to 1 inclusive, platform default 1 |
| `rate` | no | Speaking rate | Decimal string, 0.1 to 10 inclusive, platform default 1 |
| `pitch` | no | Speaking pitch | Decimal string, 0 to 2 inclusive, platform default 1 (not supported by the platform yet) |

#### Scenario: Optional parameters omitted
- **WHEN** the caller provides only `text`
- **THEN** the request is valid and the platform applies its defaults

### Requirement: Speak request payload
The client SHALL send `text` on every `TextToSpeech.speak` call, and SHALL send each optional argument (`callsign`, `language`, `voice`, `volume`, `rate`, `pitch`) only when the caller provided it.

#### Scenario: Only text provided
- **WHEN** the caller calls speak with text only
- **THEN** the request contains exactly one param, `text`

#### Scenario: Some optional arguments provided
- **WHEN** the caller provides `language` and `rate`
- **THEN** the request contains `text`, `language` and `rate` and no other optional keys

#### Scenario: Empty string is a provided value
- **WHEN** the caller provides an empty string for `callsign` or `voice`
- **THEN** the request still contains that key with the empty string

#### Scenario: Wire key spelling
- **WHEN** the caller provides a call sign
- **THEN** the request key is `callsign`

### Requirement: Speech id requests
The client SHALL send the caller's id as the `speechid` param for `TextToSpeech.pause`, `TextToSpeech.resume`, `TextToSpeech.cancel` and `TextToSpeech.getspeechstate`.

#### Scenario: Pause a speech
- **WHEN** the caller calls pause with speech id 1
- **THEN** the request is `TextToSpeech.pause` with `speechid` 1

### Requirement: Request arguments are validated
The client SHALL return `InvalidParams`, without making a platform request, when:
- the `language` of `TextToSpeech.listvoices`, or of `TextToSpeech.speak` when provided, is not a syntactically valid BCP 47 language tag;
- the `volume`, `rate` or `pitch` of `TextToSpeech.speak`, when provided, is not a plain unsigned decimal number (empty text, signs, exponents, spaces, `nan`, `inf` and a decimal comma are not accepted) inside its accepted range in the table above.

The client SHALL NOT check `text`, `callsign` or `voice`; the platform validates them.

#### Scenario: List voices
- **WHEN** the caller calls listVoices with "en-US"
- **THEN** the request is `TextToSpeech.listvoices` with `language` "en-US"

#### Scenario: Invalid language tag
- **WHEN** the caller calls listVoices, or speak with a `language`, using a malformed BCP 47 language tag
- **THEN** the caller receives `InvalidParams`
- **AND** no platform request is made

#### Scenario: Invalid volume, rate or pitch
- **WHEN** the caller calls speak with `volume` "1.5", `rate` "fast" or `pitch` ""
- **THEN** the caller receives `InvalidParams`
- **AND** no platform request is made

#### Scenario: Boundary values are accepted
- **WHEN** the caller calls speak with `volume` "0", `rate` "0.1" and `pitch` "2"
- **THEN** the request is sent

### Requirement: Responses are returned as typed values
The client SHALL return each response as the typed struct for that method, with every field of the response copied from the wire, including `TTS_Status` and `success`.

#### Scenario: Speak response
- **WHEN** the platform answers speak with `speechid` 1, `TTS_Status` 0, `success` true
- **THEN** the caller receives speechId 1, ttsStatus 0, success true

#### Scenario: Speech state response
- **WHEN** the platform answers getspeechstate with `speechstate` 1
- **THEN** the caller receives state IN_PROGRESS

### Requirement: Errors are returned to the caller
When the underlying request fails, the client SHALL return that error to the caller as the `Result` error and SHALL NOT return a value.

#### Scenario: Speak fails
- **WHEN** the request for speak returns a general error
- **THEN** the caller receives that same error

### Requirement: Invalid response status and state values are rejected
The client SHALL return `InvalidParams` when a `TextToSpeech.getspeechstate` response omits `speechstate`, supplies a non-integer value, or supplies an integer outside the defined range 0 to 3. For every TextToSpeech response, `TTS_Status` SHALL be an unsigned integer (see `common`) and SHALL be returned to the caller as received. Firebolt 9 defines the values 0 to 3, and the platform also sends 4 (no access, when the app is not allowed to speak after `setACL`); the client SHALL NOT restrict the value, because the caller uses `TTS_Status` to judge the call.

#### Scenario: Missing speech state
- **WHEN** the response omits `speechstate`
- **THEN** the caller receives `InvalidParams`

#### Scenario: Speech state has the wrong type
- **WHEN** the response supplies a non-integer `speechstate`
- **THEN** the caller receives `InvalidParams`

#### Scenario: Speech state is unknown
- **WHEN** the response supplies a `speechstate` outside 0 to 3
- **THEN** the caller receives `InvalidParams`

#### Scenario: TTS status the platform defines beyond Firebolt 9
- **WHEN** the platform answers speak with `TTS_Status` 4 and `success` false
- **THEN** the caller receives ttsStatus 4 and success false

#### Scenario: TTS status is not an unsigned integer
- **WHEN** a TextToSpeech response supplies `TTS_Status` -1, 1.5 or "0"
- **THEN** the caller receives `InvalidParams`

### Requirement: Event subscriptions
The client SHALL provide one subscribe function for each event in `api.yaml`. A subscription SHALL deliver the event's `speechid` to the caller's callback as a `SpeechIdEvent`, and SHALL return a `SubscriptionId`.

#### Scenario: Subscribe and receive
- **WHEN** the caller subscribes to onSpeechStart and the platform sends `{ "speechid": 1 }`
- **THEN** the callback is called once with speechId 1

#### Scenario: Unsubscribe
- **WHEN** the caller calls unsubscribe with a subscription id, or unsubscribeAll
- **THEN** the matching callbacks are no longer called

### Requirement: Platform speech semantics are not enforced by the client
The client SHALL forward speak, pause, resume and cancel to the platform without adding its own state handling. The platform is documented (in OpenRPC) to behave as follows.

#### Scenario: Speak interrupts ongoing speech
- **WHEN** speak is called while speech is ongoing
- **THEN** the ongoing speech is interrupted and onSpeechInterrupted is raised

#### Scenario: Pause and resume
- **WHEN** pause is called on ongoing speech
- **THEN** onSpeechPause is raised
- **AND** pause on already paused speech does nothing

#### Scenario: Resume
- **WHEN** resume is called on paused speech
- **THEN** onSpeechResume is raised
- **AND** resume on speech that is not paused does nothing

#### Scenario: Cancel
- **WHEN** cancel is called while the utterance is being spoken
- **THEN** speech stops and onSpeechInterrupted is raised
