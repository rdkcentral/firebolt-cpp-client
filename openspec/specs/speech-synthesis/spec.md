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
