# text-to-speech Specification

## Purpose
Behaviour of the deprecated `TextToSpeech` module that its contract does not express.

## Requirements

### Requirement: TTS_Status is not interpreted
The client SHALL return `TTS_Status` and `success` exactly as received, because Firebolt 9 says the caller judges a call by `TTS_Status`, not by `success`. A `TTS_Status` other than 0, including 4 (the platform's "no access after setACL"), SHALL NOT become an error.

#### Scenario: Successful speak
- **WHEN** the platform answers `TextToSpeech.speak` with `speechid` 1, `TTS_Status` 0 and `success` true
- **THEN** the caller receives speechId 1, ttsStatus 0 and success true

#### Scenario: Status beyond Firebolt 9
- **WHEN** the platform answers `TextToSpeech.speak` with `TTS_Status` 4 and `success` false
- **THEN** the caller receives ttsStatus 4 and success false as a value, not an error
