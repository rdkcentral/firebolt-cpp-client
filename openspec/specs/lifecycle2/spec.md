# lifecycle2 Specification

## Purpose
Behaviour of the `ILifecycle` client for the wire module `Lifecycle2`: closing the app, reading its lifecycle state, and receiving state changes. This includes the client's own reconciliation after the connection resumes. See `common` for general rules.

## Requirements

### Requirement: Close request
`close` SHALL send the close type as its wire text in the `type` param.

#### Scenario: Close with deactivate
- **WHEN** the caller calls close with DEACTIVATE
- **THEN** the request is `Lifecycle2.close` with `type` "deactivate"

### Requirement: State changes are delivered as a list
A state subscription SHALL deliver a list of `StateChange` (`oldState`, `newState`) in the order received from the platform.

#### Scenario: One transition
- **WHEN** the platform sends one transition from initializing to active
- **THEN** the callback is called once with a list of one change

### Requirement: Reconcile after resume
When the connection resumes and at least one subscription exists and a state was already delivered, the client SHALL read the current state, and if it differs from the last delivered state, SHALL deliver one `StateChange` from the last delivered state to the current state to every active subscriber exactly once.

#### Scenario: State changed while disconnected
- **WHEN** the last delivered state was ACTIVE, the connection resumes and the platform reports SUSPENDED
- **THEN** every subscriber receives one change ACTIVE to SUSPENDED

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
When the platform later sends the same transition that the client already delivered during reconcile, the client SHALL NOT deliver it a second time to any subscriber. A genuinely new transition SHALL still be delivered.

#### Scenario: Late duplicate
- **WHEN** the client delivered ACTIVE to SUSPENDED during reconcile and the platform then sends ACTIVE to SUSPENDED
- **THEN** that change is not delivered again

#### Scenario: Different transition first
- **WHEN** the client delivered ACTIVE to SUSPENDED during reconcile and the platform then sends SUSPENDED to ACTIVE followed by ACTIVE to SUSPENDED
- **THEN** both changes are delivered

### Requirement: Callback and unsubscribe safety
An exception thrown by one subscriber's callback SHALL NOT stop delivery to other subscribers. A callback SHALL be able to wait for another thread that is unsubscribing without deadlock. After `unsubscribe` or `unsubscribeAll` the removed subscribers SHALL NOT be called again, and reconcile state SHALL be cleared.

#### Scenario: Callback throws
- **WHEN** one subscriber's callback throws during delivery
- **THEN** the other subscribers still receive the change and a warning is logged

#### Scenario: Unsubscribe and subscribe again
- **WHEN** all subscriptions are removed and the caller subscribes again
- **THEN** no earlier delivered state or pending suppression carries over
