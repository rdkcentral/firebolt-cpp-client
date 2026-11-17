# stats Specification

## Purpose
Behaviour of the `IStats` client: container memory usage in bytes. See `common` for general rules.

## Requirements

### Requirement: Memory usage fields
`memoryUsage` SHALL return `userMemoryUsed`, `userMemoryLimit`, `gpuMemoryUsed` and `gpuMemoryLimit` as received, and SHALL return an error when the response is not the expected object.

#### Scenario: Complete response
- **WHEN** the platform answers with the four values
- **THEN** the caller receives all four values

#### Scenario: Bad response
- **WHEN** the platform answers with something that is not the expected object
- **THEN** the caller receives an error
