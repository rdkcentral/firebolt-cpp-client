# metrics Specification

## Purpose
Behaviour of the `Metrics` module that its contract does not express.

## Requirements

### Requirement: Error type is checked
`error` SHALL return `InvalidParams` without sending a request when `type` is not a defined `ErrorType`.

#### Scenario: Defined error type
- **WHEN** the caller calls `error` with `Network`, a code, a description and `visible` true
- **THEN** the request is `Metrics.error` with `type` "network" and no `parameters`

#### Scenario: Undefined error type
- **WHEN** the caller passes an `ErrorType` value outside the defined set
- **THEN** the caller receives `InvalidParams` and no request is sent
