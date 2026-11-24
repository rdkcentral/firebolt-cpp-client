# stats Specification

## Purpose
Behaviour of the `IStats` client: container memory usage in bytes. See `common` for general rules.

## Requirements

### Requirement: Memory usage fields
`memoryUsage` SHALL return `userMemoryUsed`, `userMemoryLimit`, `gpuMemoryUsed` and `gpuMemoryLimit` as received, and SHALL return an error when the response is not the expected object.

### Requirement: Values are 64-bit unsigned integers
Firebolt 9 types the four values as 32-bit `unsigned`, but they are byte counts, which can exceed 4 GiB (the platform sends them as 64-bit integers), so the client SHALL accept any non-negative JSON integer up to 2^64 - 1. A negative number, a fraction, a boolean or text SHALL be an error (`InvalidParams`) and SHALL NOT wrap or truncate.

#### Scenario: Value above 4 GiB
- **WHEN** the platform answers `userMemoryUsed` 8589934592
- **THEN** the caller receives 8589934592

#### Scenario: Negative value
- **WHEN** the platform answers a value of -1
- **THEN** the caller receives `InvalidParams`

#### Scenario: Complete response
- **WHEN** the platform answers with the four values
- **THEN** the caller receives all four values

#### Scenario: Bad response
- **WHEN** the platform answers with something that is not the expected object
- **THEN** the caller receives an error
