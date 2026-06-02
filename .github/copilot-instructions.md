# firebolt-cpp-client Copilot Instructions

Scope: This file applies to the firebolt-cpp-client repository.

## Primary goals

- Preserve API contract correctness across interface, implementation, tests, and OpenRPC fixtures.
- Keep generated surfaces and bespoke conventions aligned.
- Prefer minimal, targeted changes.

## High-signal workflow

1. For API-facing changes, update all of the following in one pass:
   - `include/firebolt/*.h`
   - `src/*_impl.h` and `src/*_impl.cpp`
   - `test/unit/*Test.cpp` and `test/component/*Test.cpp`
   - `docs/openrpc/the-spec/firebolt-open-rpc.json` when component tests depend on fixture shape.
2. Run component tests after edits.
3. If behavior is generator-owned, patch generator code in sibling repo and regenerate module artifacts.

## Test commands

- Local one-shot (current preferred):
  - `./run-component-tests-local.sh`
  - `./run-component-tests-local.sh --skip-image-build`
- Legacy wrappers may still exist in conversation history; prefer the local script in this repo.
- Unit-only:
  - `./run-unit-tests.sh`

## Actions module rules (important)

- `Actions.intent` is getter-only:
  - takes no parameters
  - returns `Result<std::string>`
- `Actions.onIntent` callback payload is a string value.
- Component event trigger for `Actions.onIntent` should use a string JSON payload (for example `"launch"`), not an object.

## Generated-code conventions that must be preserved

- `*Impl` classes should delete copy constructor and copy assignment:
  - `ClassName(const ClassName&) = delete;`
  - `ClassName& operator=(const ClassName&) = delete;`
- Unless explicitly justified as safe, `*Impl` classes should also delete move operations:
  - `ClassName(ClassName&&) = delete;`
  - `ClassName& operator=(ClassName&&) = delete;`
- Keep include hygiene strict:
  - include `<utility>` when using `std::move`
  - remove unused includes such as `<regex>` when not used
- Keep test names in consistent CamelCase for filtering.

## Component test expectations

- Red schema validation lines in logs can be expected for negative-path tests.
- Negative tests must still verify runtime behavior (callbacks not delivered for invalid payloads), not just compile-time surface checks.

## OpenRPC fixture expectations

- Module descriptions must match actual API behavior.
- Getter-style methods should carry property tags consistent with the rest of the file (for example `property:readonly` where applicable).
- Keep notifier/subscriber metadata aligned (`x-notifier`, `x-subscriber-for`).

## Regeneration notes (sibling repo)

When a change is generator-owned, use `firebolt-sdk-gen` and apply module-scoped output back into this repo.

Typical flow:

- From `../firebolt-sdk-gen`:
  - `./sync-plan-checklist.sh --profile core --module actions --apply --no-accessor-touchpoints --target-root ../firebolt-cpp-client`

This keeps migration incremental and avoids unrelated accessor touchpoint churn.

## CI parity reminders

- CI uses Dockerized build/test flow and mock-firebolt integration.
- Keep changes compatible with:
  - `.github/workflows/ci.yml`
  - `.github/scripts/run-component-tests.sh`

## PR hygiene

- If a review asks for include-file fixes, prefer precise header/source edits and re-run component tests.
- Do not relax negative tests just to suppress red validation logs.
- Keep commit messages scoped and explicit (example: `fix(actions): address include review comments`).
