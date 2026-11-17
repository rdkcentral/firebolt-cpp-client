# text-to-speech Specification

## Purpose
Behaviour of the C++ TextToSpeech client (`ITextToSpeech`). Method names, params, types and wire names are in `api.yaml` and are not repeated here. Methods are referred to by their wire name, for example `TextToSpeech.speak`.

## Requirements

### Requirement: Speak request payload
The client SHALL send `text` on every `TextToSpeech.speak` call, and SHALL send each optional argument (`callSign`, `language`, `voice`, `volume`, `rate`, `pitch`) only when the caller provided it.

#### Scenario: Only text provided
- **WHEN** the caller calls speak with text only
- **THEN** the request contains exactly one param, `text`

#### Scenario: Some optional arguments provided
- **WHEN** the caller provides `language` and `rate`
- **THEN** the request contains `text`, `language` and `rate` and no other optional keys

#### Scenario: Empty string is a provided value
- **WHEN** the caller provides an empty string for `callSign` or `rate`
- **THEN** the request still contains that key with the empty string

#### Scenario: Wire key spelling
- **WHEN** the caller provides a call sign
- **THEN** the request key is `callSign`, not `callsign`

### Requirement: Speech id requests
The client SHALL send the caller's id as the `speechid` param for `TextToSpeech.pause`, `TextToSpeech.resume`, `TextToSpeech.cancel` and `TextToSpeech.getspeechstate`.

#### Scenario: Pause a speech
- **WHEN** the caller calls pause with speech id 1
- **THEN** the request is `TextToSpeech.pause` with `speechid` 1

### Requirement: Voice list request
The client SHALL send the caller's language as the `language` param for `TextToSpeech.listvoices`.

#### Scenario: List voices
- **WHEN** the caller calls listVoices with "en-US"
- **THEN** the request is `TextToSpeech.listvoices` with `language` "en-US"

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
