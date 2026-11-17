# presentation Specification

## Purpose
Behaviour of the `IPresentation` client: whether the app is focused. See `common` for general rules.

## Requirements

### Requirement: Focus is a boolean
`focused` SHALL return the platform's boolean and SHALL return an error when the response is not a boolean.

#### Scenario: Focused
- **WHEN** the platform answers `Presentation.focused` with true
- **THEN** the caller receives true

#### Scenario: Bad response
- **WHEN** the platform answers with something that is not a boolean
- **THEN** the caller receives an error
