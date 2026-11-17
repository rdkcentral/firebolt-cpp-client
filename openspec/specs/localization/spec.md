# localization Specification

## Purpose
Behaviour of the `ILocalization` client: country, languages and time zone settings and their change events. See `common` for general rules.

## Requirements

### Requirement: Values returned as received
`country`, `presentationLanguage` and `timeZone` SHALL return the platform's text unchanged, and `preferredAudioLanguages` SHALL return the platform's list of language codes.

#### Scenario: Time zone
- **WHEN** the platform answers `Localization.timeZone` with "America/New_York"
- **THEN** the caller receives "America/New_York"

#### Scenario: Bad response
- **WHEN** the platform answers a getter with the wrong JSON type
- **THEN** the caller receives an error
