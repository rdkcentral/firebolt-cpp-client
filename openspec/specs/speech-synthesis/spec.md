# speech-synthesis Specification

## Purpose
Behaviour of the `ISpeechSynthesis` client: speaking utterances, listing voices and receiving utterance events. See `common` for general rules.

## Requirements

### Requirement: Speak payload
`speak` SHALL send `text` always, and SHALL send `lang`, `voice`, `volume`, `rate`, `pitch` and `pii` only when the caller provided them. It SHALL return the utterance id as an unsigned number.

#### Scenario: All arguments provided
- **WHEN** the caller provides text "payload", lang "en-US", voice "Salli", volume 0.8, rate 1.25, pitch 1.1 and pii false
- **THEN** the request is `SpeechSynthesis.speak` with exactly those seven params and the caller receives the returned id

#### Scenario: Id is not a number
- **WHEN** the platform answers speak with something that is not a number
- **THEN** the caller receives an error

### Requirement: Voices request
`voices` SHALL send no params and SHALL return the list of voices with `name`, `lang` and `default`.

#### Scenario: Voices response
- **WHEN** the platform answers with an array of one voice
- **THEN** the caller receives a list of one voice

#### Scenario: Response is not an array
- **WHEN** the platform answers with an object instead of an array
- **THEN** the caller receives an error

### Requirement: Utterance control
`cancel`, `pause` and `resume` SHALL send the caller's id as the `utteranceId` param and SHALL return no value; a request error SHALL be returned to the caller.

#### Scenario: Pause
- **WHEN** the caller calls pause with utterance id 1
- **THEN** the request is `SpeechSynthesis.pause` with `utteranceId` 1

### Requirement: Utterance events
An utterance event SHALL carry `utteranceId` and an `event` from the known set (`synthesisStarting`, `playbackStarting`, `paused`, `resumed`, `completed`, `interrupted`, `networkFailed`, `synthesisFailed`, `playbackFailed`). The client SHALL reject a payload with an unknown `event` or with the old `id` field in place of `utteranceId`.

#### Scenario: Valid event
- **WHEN** the platform sends `utteranceId` 77 and `event` "resumed"
- **THEN** the callback receives id 77 and event `resumed`

#### Scenario: Unknown event
- **WHEN** the platform sends an event name that is not in the known set
- **THEN** the payload is rejected

### Requirement: Platform errors pass through
The client SHALL return platform errors unchanged and SHALL NOT create them. Firebolt 9 defines the specific errors -50110 (app state invalid, requires focus; `speak`, `resume`) and -50130 (speech synthesis engine error; `speak`, `cancel`, `pause`, `resume`). `voices` SHALL return an empty list, not an error, when no speech synthesis engine is available.

#### Scenario: Engine error
- **WHEN** the platform answers `SpeechSynthesis.speak` with error -50130
- **THEN** the caller receives that same error code

#### Scenario: No engine for voices
- **WHEN** the platform answers `SpeechSynthesis.voices` with an empty list
- **THEN** the caller receives an empty list as a successful result

### Requirement: Speak arguments are checked
`speak` SHALL return `InvalidParams`, without making a platform request, when `lang` is not a syntactically valid BCP 47 tag, or when `volume` is outside 0 to 1, `rate` outside 0.1 to 10, or `pitch` outside 0 to 2 (inclusive; NaN is rejected). `text` and `voice` are not checked by the client.

#### Scenario: Value out of range
- **WHEN** the caller calls speak with `volume` 1.1
- **THEN** the caller receives `InvalidParams` and no request is sent

#### Scenario: Boundary values
- **WHEN** the caller calls speak with volume 0, rate 0.1 and pitch 2
- **THEN** the request is sent
