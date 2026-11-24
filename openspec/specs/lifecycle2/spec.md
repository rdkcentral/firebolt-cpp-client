# lifecycle2 Specification

## Purpose
Behaviour of the `Lifecycle` interface (wire module `Lifecycle2`) beyond its contract: close type checking, and the client's own reconciliation of the lifecycle state after the connection resumes.

## Requirements

### Requirement: Close type is checked
`close` SHALL return `InvalidParams` without sending a request when `type` is not a defined `CloseType`.

#### Scenario: Valid close type
- **WHEN** the caller calls `close` with `DEACTIVATE`
- **THEN** the request is `Lifecycle2.close` with `type` "deactivate"

#### Scenario: Undefined close type
- **WHEN** the caller passes a `CloseType` value outside the defined set
- **THEN** the caller receives `InvalidParams` and no request is sent

### Requirement: Reconcile after resume
When the connection resumes, and at least one subscription exists, and a state was already delivered, the client SHALL read the current state. If it differs from the last delivered state, the client SHALL deliver one `StateChange` from the last delivered state to the current state to every subscriber, once.

#### Scenario: State changed while disconnected
- **WHEN** the last delivered state was ACTIVE, the connection resumes and the platform reports SUSPENDED
- **THEN** every subscriber receives one change from ACTIVE to SUSPENDED

#### Scenario: State unchanged
- **WHEN** the connection resumes and the platform reports the last delivered state
- **THEN** nothing is delivered

#### Scenario: Nothing to reconcile
- **WHEN** the connection resumes and there are no subscribers, or no state has been delivered yet
- **THEN** no state request is made

#### Scenario: State read fails
- **WHEN** the connection resumes and reading the state returns an error
- **THEN** nothing is delivered and the last delivered state is kept

### Requirement: No duplicate after reconcile
When the platform later sends the transition that the client already delivered during reconcile, the client SHALL NOT deliver it again to any subscriber. A genuinely new transition SHALL still be delivered.

#### Scenario: Late duplicate
- **WHEN** the client delivered ACTIVE to SUSPENDED during reconcile and the platform then sends ACTIVE to SUSPENDED
- **THEN** that change is not delivered again

#### Scenario: Different transition first
- **WHEN** the client delivered ACTIVE to SUSPENDED during reconcile and the platform then sends SUSPENDED to ACTIVE followed by ACTIVE to SUSPENDED
- **THEN** both changes are delivered

### Requirement: Callback and unsubscribe safety
An exception thrown by one subscriber's callback SHALL NOT stop delivery to the other subscribers, and a warning SHALL be logged. A callback SHALL be able to wait for another thread that is unsubscribing without deadlock. After `unsubscribe` or `unsubscribeAll`, removed subscribers SHALL NOT be called and the reconcile state SHALL be cleared.

#### Scenario: Callback throws
- **WHEN** one subscriber's callback throws during delivery
- **THEN** the other subscribers still receive the change

#### Scenario: Unsubscribe and subscribe again
- **WHEN** all subscriptions are removed and the caller subscribes again
- **THEN** no earlier delivered state or pending duplicate suppression carries over
