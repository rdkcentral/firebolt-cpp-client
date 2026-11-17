# accessibility Specification

## Purpose
Behaviour of the `IAccessibility` client: device accessibility settings and their change events. See `common` for general rules.

## Requirements

### Requirement: Voice guidance settings need every field
The client SHALL treat `enabled`, `rate` and `navigationHints` as required in voice guidance settings.

#### Scenario: Partial voice guidance settings
- **WHEN** the platform answers `Accessibility.voiceGuidanceSettings` with only `enabled`
- **THEN** the caller receives the error `InvalidParams`

### Requirement: Closed captions settings need every field
The client SHALL treat `enabled` and `preferredLanguages` as required in closed captions settings (Firebolt 9 always returns both; `preferredLanguages` is `[]` when not initialized).

#### Scenario: Partial closed captions settings
- **WHEN** the platform answers `Accessibility.closedCaptionsSettings` with only `enabled`
- **THEN** the caller receives an error
