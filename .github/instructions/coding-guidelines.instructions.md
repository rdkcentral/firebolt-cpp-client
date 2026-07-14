---
applyTo: "**/*.h,**/*.cpp,**/CMakeLists.txt"
---

# firebolt-cpp-client — Coding Guidelines

**Scope:** This document governs code generation and modification for the `firebolt-cpp-client` repository.
It is intended for use by both AI agents (Copilot, openspec) and human developers.

**How to read this document:**
- **Current practice** — observed consistently in the codebase; enforce as-is.
- **Recommended going forward** — not yet consistent but must be adopted for new/modified code.
- **Anti-pattern** — seen in the repo OR likely to be introduced by AI generation; includes why it is wrong in this specific codebase.

Markers:
- `[ASSUMPTION]` — inferred from code patterns where no explicit policy exists.

---

## 1. Architecture and Module Boundaries

### 1.1 Three-Layer Architecture

The codebase has exactly three conceptual layers. Do not collapse or skip layers.

| Layer | Location | Purpose |
|---|---|---|
| Public API | `include/firebolt/<module>.h` | Pure virtual interfaces; the only surface consumers see |
| Implementation | `src/<module>_impl.h` + `src/<module>_impl.cpp` | Concrete logic; hidden from consumers |
| JSON deserialization | `src/json_types/<module>.h` | Wire format ↔ native type adapters; never exposed publicly |

**Current practice (confirmed across all 13 modules):**
- Every module has exactly one interface (`I<Module>`), one impl (`<Module>Impl`), and zero or more JSON type adapters. Four modules (discovery, localization, network, presentation) have no `json_types/` file; they use primitive-type helpers (`Firebolt::JSON::String`, `Boolean`, `Unsigned`) directly.
- The singleton entry point is `Firebolt::IFireboltAccessor::Instance()`, implemented in `src/firebolt.cpp` as a Meyers singleton over `FireboltAccessorImpl`.

**Anti-patterns:**
- Do not add business logic to `src/json_types/*.h` files. They must only do: field validation, field extraction, enum mapping.
- Do not add `#include <firebolt/<module>.h>` inside another module's public header unless there is a direct type dependency. Cross-module dependencies at the public layer risk coupling the consumer's include graph. `include/firebolt/metrics.h` correctly includes `"firebolt/common_types.h"` because it uses `AgePolicy`.
- Do not add new public `include/firebolt/` headers for internal types. Internal types live in `src/`.

### 1.2 Module Registration

Every new module must be wired into `src/firebolt.cpp`:
1. Member variable of type `Module::ModuleImpl` in `FireboltAccessorImpl`.
2. Initialised via `Firebolt::Helpers::GetHelperInstance()` in the constructor initialiser list.
3. Accessor method returning `Module::IModule&` override.
4. `unsubscribeAll()` call in the private `unsubscribeAll()` helper — **only if the module supports subscriptions** (i.e., the interface exposes any `subscribeOn*` methods). Note: `Device` currently exposes `subscribeOnHdrChanged(...)` but is absent from `FireboltAccessorImpl::unsubscribeAll()` — see Known Issue below.

Reference: `src/firebolt.cpp` (`FireboltAccessorImpl` ctor initializer list and `unsubscribeAll()`).

**Known issue:** `FireboltAccessorImpl::unsubscribeAll()` in `src/firebolt.cpp` does not call `device_.unsubscribeAll()` even though `DeviceImpl` supports subscriptions (`subscribeOnHdrChanged`, `subscribeOnDolbyAtmosExperienceAvailableChanged`). Device subscriptions are never cleaned up on Disconnect. Track this as a separate defect.

### 1.3 API-Facing Change Discipline

When making an API-facing change — adding or modifying an interface method, changing a return type, or adding a subscription — update all four artifacts in a single commit:
1. `include/firebolt/<module>.h`
2. `src/<module>_impl.h` and `src/<module>_impl.cpp`
3. `test/unit/<module>Test.cpp` and `test/component/<module>Test.cpp`
4. `docs/openrpc/the-spec/firebolt-open-rpc.json` — when component tests depend on fixture shape.

Never leave the three layers out of sync after a commit. A compile-passing diff that omits a test update or fixture update is incomplete.

After the change, run `./run-component-tests-local.sh` to validate all layers together before pushing.

---

## 2. Naming Conventions

### 2.1 Namespaces

**Current practice (confirmed in every file):**
- Top-level namespace: `Firebolt`
- Module namespace: `Firebolt::<ModuleName>` where `<ModuleName>` matches the directory/header name with initial capital (e.g., `Firebolt::Device`, `Firebolt::Lifecycle`, `Firebolt::TextToSpeech`).
- JSON adapter namespace: `Firebolt::<ModuleName>::JsonData` for module-local types (e.g., `Firebolt::Device::JsonData`, `Firebolt::Accessibility::JsonData`).
- Cross-module shared JSON types: `Firebolt::JsonData` (e.g., `AgePolicyEnum` in `src/json_types/common.h`).
- Every `.h` and `.cpp` file closes with a namespace-end comment: `} // namespace Firebolt::<ModuleName>`.

**Anti-patterns:**
- Do not use `using namespace Firebolt::Helpers;` (or `using namespace Firebolt::<Module>;`) in `.cpp` files. Prefer fully-qualified names or targeted `using Firebolt::Helpers::ClassName;` declarations instead. (Applies retroactively to `stats_impl.cpp` and `lifecycle_impl.cpp` — both currently have `using namespace Firebolt::Helpers;` at file scope — flagged for cleanup.)

### 2.2 Interfaces and Implementations

**Current practice:**
- Interface: `I<ModuleName>` declared in `include/firebolt/<module>.h` (e.g., `IDevice`, `ILifecycle`, `IActions`).
- Implementation: `<ModuleName>Impl` in `src/<module>_impl.h` (e.g., `DeviceImpl`, `LifecycleImpl`).
- Neither the interface nor the impl class is named simply `<Module>` — that name is reserved for the namespace.

### 2.3 Method Names

**Current practice (confirmed across all modules):**
- Getter methods: lowerCamelCase, no `get` prefix (e.g., `chipsetId()`, `audioDescription()`, `connected()`).
- Subscription methods: `subscribeOn<EventName>()` returning `Result<SubscriptionId>` (e.g., `subscribeOnHdrChanged`, `subscribeOnCountryChanged`, `subscribeOnIntent`).
- Unsubscription: `unsubscribe(SubscriptionId id)` (universal, module-scoped) and `unsubscribeAll()`.
- Invoke-style (fire-and-forget): verb phrases matching the RPC method (e.g., `ready()`, `signIn()`, `close()`).

**Anti-patterns:**
- Do not name subscription methods `subscribe<EventName>()` without the `On` prefix — it violates the established naming scheme.
- Do not use `Get`, `Set`, `Is` prefixes on getter methods.

### 2.4 RPC Method and Event Name Strings

**Current practice:**
- Getter/invoke RPC name: `"<Module>.<methodName>"` in camelCase (e.g., `"Device.chipsetId"`, `"Metrics.startContent"`).
- Event name: `"<Module>.on<EventName>"` (e.g., `"Device.onHdrChanged"`, `"Actions.onIntent"`, `"Lifecycle2.onStateChanged"`).
- `Lifecycle2` is the correct wire name for lifecycle RPCs in this version — do not use `Lifecycle`.
- TextToSpeech event names use lowercase suffixes matching the wire protocol (e.g., `"TextToSpeech.onWillspeak"`, `"TextToSpeech.onSpeechstart"`). These must match the OpenRPC fixture exactly.

**Anti-patterns:**
- Do not guess RPC or event string names. Always derive them from `docs/openrpc/the-spec/firebolt-open-rpc.json`.

### 2.5 Enum Names

**Current practice:**
- C++ enum class names: `SCREAMING_SNAKE_CASE` (e.g., `INITIALIZING`, `ACTIVE`, `KILL_RELOAD`).
- `EnumType` instance variable names: `<EnumClass>Enum` (e.g., `LifecycleStateEnum`, `CloseReasonEnum`, `DeviceClassEnum`, `AgePolicyEnum`).
- Wire values in `EnumType` map: lowercase strings matching the OpenRPC fixture (e.g., `{"initializing", ...}`, `{"killReload", ...}`).

### 2.6 JSON Adapter Classes

**Current practice:**
- Struct adapters: `class <TypeName> : public Firebolt::JSON::NL_Json_Basic<::<Firebolt::Module::NativeType>>` (e.g., `HDRFormat`, `ClosedCaptionsSettings`, `StateChange`).
- Enum adapter instantiation: `inline const Firebolt::JSON::EnumType<NativeEnum> <EnumName>Enum({{...}})` at namespace scope.

### 2.7 Test Class Names

**Current practice (partial inconsistency — see note):**
- Unit test class: `<Module>UTest` inheriting `::testing::Test` and `MockBase` (e.g., `DeviceUTest`, `AccessibilityUTest`).
- Component test class: `<Module>CTest` inheriting `::testing::Test` (e.g., `DeviceCTest`, `LifecycleCTest`).
- `ActionsGeneratedUTest` in `test/unit/actionsGeneratedTest.cpp` uses a non-standard name because it is auto-generated. This is acceptable only for auto-generated test files.

**Recommended going forward:** New manually-authored test files must follow the `<Module>UTest` / `<Module>CTest` naming.

---

## 3. Header Guards and Include Style

### 3.1 Header Guards

**Current practice:**
- Bespoke headers (`include/firebolt/` and most `src/`): use `#pragma once`.
- Auto-generated **interface and impl** headers (e.g., `include/firebolt/actions.h`, `src/actions_impl.h`): use `#ifndef FIREBOLT_<MODULE>_H` / `#define FIREBOLT_<MODULE>_H` / `#endif` guards — confirmed in `include/firebolt/actions.h` and `src/actions_impl.h`.
- Auto-generated **json_types** headers (e.g., `src/json_types/actions.h`): use `#pragma once` — consistent with all bespoke json_types headers.
- Do not mix both guards in the same file.

**Recommended going forward:** All new bespoke headers use `#pragma once` exclusively. New auto-generated interface/impl headers follow the `#ifndef`/`#define`/`#endif` pattern; new auto-generated json_types headers follow `#pragma once`.

### 3.2 Include Style

**Current practice (confirmed across all 13 modules):**
- Includes of public firebolt headers: angle brackets with full path (`#include <firebolt/types.h>`, `#include <firebolt/helpers.h>`).
- Includes of implementation-local headers: double quotes without path prefix (`#include "device_impl.h"`, `#include "json_types/device.h"`).
- Includes of the module's own public header from within `_impl.h`: double quotes with full path (`#include "firebolt/device.h"`).

**Anti-pattern:** Do not `#include <firebolt/helpers.h>` in `include/firebolt/*.h` public headers. Helpers are an internal abstraction not part of the public API. Consumers must never see `IHelper`.

### 3.3 Include Hygiene

**Current practice:**
- Include `<utility>` when using `std::move`.
- Remove unused includes such as `<regex>` when not used.

Confirmed observation: `include/firebolt/actions.h` (auto-generated) includes `<utility>` at line 30 because it uses `std::move` in the `subscribeOnIntentChanged` default method body.

---

## 4. Class Structure and Copy/Move Semantics

### 4.1 `*Impl` Class Declaration Order

**Recommended going forward:**
```
class <Module>Impl : public I<Module>
{
public:
    explicit <Module>Impl(Firebolt::Helpers::IHelper& helper);
    <Module>Impl(const <Module>Impl&) = delete;
    <Module>Impl& operator=(const <Module>Impl&) = delete;
    <Module>Impl(<Module>Impl&&) = delete;
    <Module>Impl& operator=(<Module>Impl&&) = delete;
    ~<Module>Impl() override = default;  // or override with body when custom cleanup needed

    // method overrides

private:
    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;  // only if module supports subscriptions
};
```

- All `*Impl` constructors take `Firebolt::Helpers::IHelper&` as their only parameter and must be marked `explicit`. Three impl classes currently lack `explicit` — flagged for cleanup:
  - `src/stats_impl.h` — `StatsImpl(Firebolt::Helpers::IHelper&)` at line 30
  - `src/lifecycle_impl.h` — `LifecycleImpl(Firebolt::Helpers::IHelper&)` at line 37
  - `src/localization_impl.h` — `LocalizationImpl(Firebolt::Helpers::IHelper&)` at line 29
- Method return types and parameter types in `*_impl.h` must exactly match those declared in the corresponding public interface header. Always use `uint32_t` (from `<cstdint>`), never the non-standard POSIX type `u_int32_t`. A type mismatch between interface and override produces an invalid override and fails to compile on non-POSIX targets.

  **Cleanup to fix:** `src/device_impl.h` — `timeInActiveState()` is declared as `u_int32_t`; must be changed to `uint32_t` to match `include/firebolt/device.h`.

### 4.2 Deleted Copy Operations

**Current practice:** Copy constructor and copy assignment operator are explicitly deleted in **all** 13 `*Impl` classes and in `FireboltAccessorImpl` in `src/firebolt.cpp`. This is mandatory.

### 4.3 Move Operations

All `*Impl` classes and `FireboltAccessorImpl` must explicitly delete the move constructor and move assignment operator, placed immediately after the deleted copy operations:

```cpp
ClassName(ClassName&&) = delete;
ClassName& operator=(ClassName&&) = delete;
```

**Rationale:** These classes hold a non-reassignable reference member (`helper_`) and, where applicable, a `SubscriptionManager`. A compiler-generated move would leave the source object with a dangling reference or corrupted subscription state. Explicit deletion makes the intent clear and prevents accidental moves at call sites.

**Cleanup to fix:** No `*Impl` class currently deletes move operations. The following files must be updated:
- `src/accessibility_impl.h`
- `src/actions_impl.h`
- `src/advertising_impl.h`
- `src/device_impl.h`
- `src/discovery_impl.h`
- `src/display_impl.h`
- `src/lifecycle_impl.h`
- `src/localization_impl.h`
- `src/metrics_impl.h`
- `src/network_impl.h`
- `src/presentation_impl.h`
- `src/stats_impl.h`
- `src/texttospeech_impl.h`
- `src/firebolt.cpp` (`FireboltAccessorImpl`)

### 4.4 Destructor

Always declare `~<Module>Impl() override = default;`. Do not define a destructor with an empty body `{}` — an empty body is not custom cleanup and must be written as `= default`. Define a body only when it performs actual cleanup work (e.g., releasing a resource not managed by RAII).

**Cleanup to fix:**
- `src/stats_impl.h` + `src/stats_impl.cpp` — destructor is declared non-inline with an empty body; replace the declaration with `~StatsImpl() override = default;` in the header and remove the definition from the `.cpp` file.
- `src/lifecycle_impl.h` + `src/lifecycle_impl.cpp` — same issue; apply the same fix.

### 4.5 Friend Declarations

Do not use `friend` declarations to grant test classes access to implementation internals. Design the public interface to be testable. If internal state genuinely must be observed in a test, use a `protected` member with a test-only subclass. Friend declarations break encapsulation without providing a durable or type-safe test seam, and no other class in this codebase uses this pattern.

**Cleanup to fix:** `src/lifecycle_impl.h` — `friend class ::LifecycleTest;` is the only `friend` declaration in the codebase and must be removed.

---

## 5. Error Handling

### 5.1 `Result<T>` at the API Boundary

**Current practice (without exception in 13 modules):**
- Every method in `I<Module>` returns `Result<T>` or `Result<void>`.
- `Result<void>` is used for methods that send a command and carry no return value (e.g., `Metrics.ready()`, `Lifecycle.close()`).
- Callers check the result with boolean conversion (`if (result)`) or dereference after assertion (`*result`).

**Anti-pattern:** Do not throw exceptions from public interface methods. Do not return bare `T` where failure is possible. Do not use `std::optional<T>` as a substitute for `Result<T>` — `optional` cannot carry an error code.

### 5.2 Error Propagation in Implementations

**Current practice:** Implementations propagate errors by returning the result of `helper_.get<>()`, `helper_.invoke()`, or `subscriptionManager_.subscribe<>()` directly. No intermediate `try-catch` is present in any `*_impl.cpp` file.

**Anti-pattern:** Do not add `try-catch` blocks in `*_impl.cpp` for errors the helper/transport already handles. Do not swallow errors silently.

### 5.3 Error Handling in JSON Adapters

**Current practice (confirmed in `src/json_types/`):**
- `fromJson()` throws `std::invalid_argument("Missing required fields in JSON")` when required fields are absent. This is caught by the framework and converted to `Result<T>` with `Error::InvalidParams`.
- `EnumType::at()` throws when an unknown wire value is encountered — also caught by the framework.
- Do not use `Result<T>` inside `fromJson()`. Throw only.

---

## 6. JSON Deserialization Layer (`src/json_types/`)

### 6.1 JSON Type File Rules

**Current practice:**
- Each `src/json_types/<module>.h` includes its corresponding `include/firebolt/<module>.h` and `<firebolt/json_types.h>`.
- No `.cpp` file exists under `src/json_types/` — all JSON adapter logic is header-only.
- JSON adapter classes are defined in the `Firebolt::<Module>::JsonData` namespace.

### 6.2 Struct Adapters

**Current practice:**
```cpp
class <TypeName> : public Firebolt::JSON::NL_Json_Basic<::<Firebolt::Module::NativeType>>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        if (!checkRequiredFields(json, {"field1", "field2"}))
        {
            throw std::invalid_argument("Missing required fields in JSON");
        }
        field1_ = json["field1"].get<CppType>();
        field2_ = json["field2"].get<CppType>();
    }
    ::<Firebolt::Module::NativeType> value() const override
    {
        return ::<Firebolt::Module::NativeType>{field1_, field2_};
    }
private:
    CppType field1_;
    CppType field2_;
};
```

Reference: `src/json_types/accessibility.h` (`ClosedCaptionsSettings`, `VoiceGuidanceSettings`), `src/json_types/device.h` (`HDRFormat`), `src/json_types/lifecycle.h` (`StateChange`).

### 6.3 Enum Adapters

**Current practice:**
```cpp
inline const Firebolt::JSON::EnumType<::<NativeEnum>> <EnumName>Enum({
    {"wire-string", ::<NativeEnum>::ENUMERATOR},
    ...
});
```
Wire strings are lowercase or camelCase matching the OpenRPC fixture exactly.

Reference: `src/json_types/lifecycle.h` (`LifecycleStateEnum`, `CloseReasonEnum`), `src/json_types/device.h` (`DeviceClassEnum`), `src/json_types/common.h` (`AgePolicyEnum`).

### 6.4 Unit Conversion in JSON Adapters

**Current practice (specific to `Stats` module):**
The wire payload uses `*KiB` field names (e.g., `userMemoryUsedKiB`). The JSON adapter in `src/json_types/stats.h` reads the raw KiB values and the public API returns those values in KiB units. Tests in `test/unit/statsTest.cpp` and `test/component/statsTest.cpp` validate against the fixture's raw KiB values.

**Anti-pattern:** Do not add unit conversion (e.g., ×1024 for KiB→bytes) inside `fromJson()` without a corresponding change to the public API type, tests, and OpenRPC fixture annotation.

---

## 7. Helper Abstraction Usage

### 7.1 `IHelper` Injection

**Current practice:** All `*Impl` constructors accept `Firebolt::Helpers::IHelper&` by reference and store it in `helper_`. This allows the unit test `MockHelper` to be injected without a virtual wrapper on the Impl class itself.

**Anti-pattern:** Do not accept `IHelper*` (pointer) — the codebase consistently uses references. Do not store a copy of the helper.

### 7.2 `helper_.get<JsonType, NativeType>(methodName)` — Getter Methods

**Current practice:**
- No-parameter getters: `helper_.get<Firebolt::JSON::String, std::string>("Module.method")`
- Primitive types: use `Firebolt::JSON::String`, `Firebolt::JSON::Boolean`, `Firebolt::JSON::Unsigned`, etc.
- Struct types: use the module's `JsonData` class (e.g., `JsonData::HDRFormat`)
- Array types: use `Firebolt::JSON::NL_Json_Array<JsonElement, NativeElement>` (e.g., `Localization.preferredAudioLanguages`)

Reference: `src/device_impl.cpp`, `src/localization_impl.cpp`.

### 7.3 `helper_.invoke(methodName, params)` — Fire-and-Forget Methods

**Current practice:** Used for methods that return `Result<void>`. Parameters are constructed as `nlohmann::json` before the call. Optional parameters are conditionally added.

Reference: `src/metrics_impl.cpp` (all methods), `src/lifecycle_impl.cpp` (`close()`).

### 7.4 `subscriptionManager_.subscribe<JsonType>(eventName, notification)` — Subscriptions

**Current practice:**
- `JsonType` is the JSON adapter class, not the native type.
- `notification` is moved via `std::move()`.
- The `subscriptionManager_` is only present if the module exposes subscription methods.

Reference: `src/accessibility_impl.cpp`, `src/actions_impl.cpp`, `src/lifecycle_impl.cpp`.

---

## 8. Threading and Async Patterns

### 8.1 Implementation Files

**Current practice:** No threading primitives (`std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`) appear in any `*_impl.cpp` file. All async behaviour is delegated to the transport layer via `IHelper`.

**Anti-pattern:** Do not introduce thread management in `*Impl` classes. The transport manages its own threading.

### 8.2 Component Tests

**Current practice (confirmed in all event-bearing component tests):**
```cpp
class ModuleCTest : public ::testing::Test
{
protected:
    void SetUp() override { eventReceived = false; }
    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived;
};
```
Event delivery uses `cv.wait_for(lock, EventWaitTime, [&] { return eventReceived; })` via `verifyEventReceived()` and `verifyEventNotReceived()` from `test/utils.h`. `EventWaitTime` is `std::chrono::seconds(2)` (defined in `test/utils.cpp`).

**Current practice — triggering events:**
- String payload: `triggerEvent("Module.onEvent", R"("string_value")")` — note outer double-quotes in JSON
- Object payload: `triggerEvent("Module.onEvent", R"({"field": value})")`
- For `Actions.onIntent`, the payload is a JSON-encoded object: `triggerEvent("Actions.onIntent", R"({"intent":"launch","intentId":1})")`. The callback receives this string and must parse it with `nlohmann::json::parse()`. See §12.4 for the full contract.

Reference: `test/component/actionsGeneratedTest.cpp`, `test/component/deviceTest.cpp`, `test/component/networkTest.cpp`.

---

## 9. Logging

**Current practice:** `FIREBOLT_LOG_NOTICE("Client", "Version: %s", Version::String)` appears only in `src/firebolt.cpp` at connection time. No logging macros appear in individual module `*_impl.cpp` files.

**[ASSUMPTION]** The logging macro originates from the `FireboltTransport` dependency, not from this repo. Individual module implementations intentionally do not log.

**Anti-pattern:** Do not add `std::cout`, `printf`, or `FIREBOLT_LOG_*` calls to `*_impl.cpp` files. Diagnostic output in component tests uses `std::cout` only — this is test-scoped and acceptable.

---

## 10. Testing Patterns

### 10.1 Unit Tests

**Current practice:**
- Location: `test/unit/<module>Test.cpp`
- Uses `MockHelper` (GMock) via `MockBase` from `test/unit/mock_helper.h`.
- Test fixture: `class <Module>UTest : public ::testing::Test, protected MockBase`.
- Impl is instantiated directly: `Firebolt::<Module>::<Module>Impl impl_{mockHelper};`
- OpenRPC fixture is read via `JsonEngine` from `MockBase`.

**Test case rules (confirmed across all unit test files):**
- Happy path getter: call `mock("Module.method")`, then call impl method, then `ASSERT_TRUE(result)` + value check.
- Negative path (bad wire data): call `mock_with_response("Module.method", <bad_value>)`, then `ASSERT_FALSE(result)`.
- Subscribe test: call `mockSubscribe("Module.onEvent")`, subscribe, assert `ASSERT_TRUE(result)`, then call `unsubscribe` and assert success.
- Enum validation: `validate_enum("EnumName", Firebolt::<Module>::JsonData::<EnumName>Enum)` checks the fixture's schema against the code's enum map.

**Anti-pattern:** Do not test `*Impl` via `IFireboltAccessor::Instance()` in unit tests. Unit tests must isolate the impl with a mock helper, not the full singleton.

**Exception for auto-generated unit tests:** `test/unit/actionsGeneratedTest.cpp` directly instantiates `::testing::NiceMock<MockHelper>` without inheriting `MockBase`, and uses `EXPECT_CALL(mockHelper, getJson(...))` rather than the `mock()` / `mock_with_response()` convenience wrappers. This pattern is generator-owned. Do not replicate it in bespoke test files.

### 10.2 Component Tests

**Current practice:**
- Location: `test/component/<module>Test.cpp`
- Uses `Firebolt::IFireboltAccessor::Instance()` directly (live transport connection).
- Test fixture: `class <Module>CTest : public ::testing::Test` (no `MockBase`).
- Expected values derived from `jsonEngine.get_value("Module.method")` against the OpenRPC fixture.

**Event delivery tests:**
1. Subscribe with callback that sets `eventReceived = true` and calls `cv.notify_one()`.
2. Call `triggerEvent(...)`.
3. Call `verifyEventReceived(mtx, cv, eventReceived)`.
4. Unsubscribe with `verifyUnsubscribeResult(result)`.

**Negative event tests (invalid payload):**
1. Subscribe.
2. Call `triggerEvent(...)` with invalid JSON payload.
3. Call `verifyEventNotReceived(mtx, cv, eventReceived)` — callback must NOT fire.
4. Unsubscribe.

Reference: `test/component/lifecycleTest.cpp` (`subscribeOnState_JSON_RPC_compliant`).

**Component test log expectations:** Red schema validation lines in the component test log are expected and normal for negative-path tests — they indicate the transport rejected the invalid payload as intended. Do not treat them as test failures and do not suppress them by weakening the test.

Negative tests must verify runtime behaviour — specifically that callbacks are not delivered when the payload is invalid. A test that merely asserts the code compiles with an invalid type is insufficient. Always pair with `verifyEventNotReceived`. Do not relax or remove a negative test because it produces red schema validation lines.

### 10.3 Pairing Rule

**Current practice (confirmed across all 13 modules):** Every module has both a unit test file and a component test file. When adding a new module or method:
- Add unit tests in `test/unit/<module>Test.cpp`
- Add component tests in `test/component/<module>Test.cpp`
- Both test files must cover all public API methods
- Each getter/property method must have at minimum: one happy-path test and one bad-response negative test

### 10.4 Expected Values from OpenRPC

**Current practice:** Both unit and component tests derive expected values from `jsonEngine.get_value("Module.method")` (the first example in the OpenRPC fixture). Do not hardcode values that duplicate the fixture unless the value requires a type conversion (e.g., enum comparison using `static_cast<int>`).

Exception: `test/component/actionsGeneratedTest.cpp` hardcodes `"launch"` (the `intent` field) and `1` (the `intentId` field) from the fixture's `Actions.intent` / `Actions.onIntent` example result — permissible only for auto-generated files.

---

## 11. OpenRPC Fixture Alignment

### 11.1 Fixture Examples and Enum Alignment

**Current practice:**
- Fixture location: `docs/openrpc/the-spec/firebolt-open-rpc.json`
- Both unit and component test binaries read this file at runtime (path injected via `UT_OPEN_RPC_FILE` define in `test/CMakeLists.txt`).
- When adding or changing a method, the fixture must be updated to include the method, its parameters schema, and at least one example.
- Enum values in code must match `components.schemas.<EnumName>.enum` in the fixture — validated by `validate_enum()`.

**Rule:** When a component test validates against `jsonEngine.get_value("Module.method")`, the fixture's example value must produce the same result as what the live mock-firebolt instance returns. Keep these in sync.

### 11.2 Fixture Metadata Rules

**Module descriptions:** The `description` field for each module and method in the fixture must accurately describe the module's actual API behaviour. Do not copy-paste descriptions from other modules.

**Property tags:** Getter-style methods must carry a `property:readonly` tag where other getter methods in the same file use this tag. Before adding a new getter method to the fixture, verify the tagging pattern used by the surrounding methods.

**Notifier/subscriber metadata:** Subscription event entries must keep `x-notifier` and `x-subscriber-for` fields aligned with the corresponding getter or property. Adding a subscription event without updating both fields is a fixture defect.

---

## 12. Auto-Generated vs Bespoke Code

### 12.1 Auto-Generated File Recognition

Files with the following banner are owned by the `firebolt-sdk-gen` generator tool — do not modify them directly:
```
// ============================================================================
// AUTO-GENERATED by fb-gen — DO NOT EDIT
// ============================================================================
```

Confirmed auto-generated files in the repo:
- `include/firebolt/actions.h`
- `src/actions_impl.h`
- `src/actions_impl.cpp`
- `src/json_types/actions.h`
- `test/unit/actionsGeneratedTest.cpp`
- `test/component/actionsGeneratedTest.cpp`

### 12.2 Modifying Auto-Generated Output

When a change is generator-owned, use `firebolt-sdk-gen` from the sibling repo:
```bash
./sync-plan-checklist.sh --profile core --module <module> --apply --no-accessor-touchpoints --target-root ../firebolt-cpp-client
```
Do not hand-edit auto-generated files. If the generated output has a defect, fix the generator.

### 12.3 Keeping Bespoke and Generated Files Aligned

When a new bespoke module is added, ensure it follows the same structure as generated modules (`actions`) so the two styles remain similar enough that the generator could own the bespoke code in the future.

### 12.4 Actions Module API Contract

`Actions.intent` is a getter-only method: it takes no parameters and returns `Result<std::string>`. Do not add parameters to it and do not change its return type.

The `std::string` returned by `Actions.intent()` is a JSON-serialized object, not a plain scalar. Callers must parse it (confirmed in `test/component/actionsGeneratedTest.cpp`):
```cpp
auto result = accessor.ActionsInterface().intent();
ASSERT_TRUE(result);
auto parsed = nlohmann::json::parse(*result);
EXPECT_EQ(parsed.at("intent").get<std::string>(), "launch");
EXPECT_EQ(parsed.at("intentId").get<int>(), 1);
```
Do not treat the return value as a plain scalar string.

The `Actions.onIntent` callback also delivers a JSON-serialized object string. The component event trigger must use a JSON-encoded object payload (confirmed at `test/component/actionsGeneratedTest.cpp:62`):
```cpp
triggerEvent("Actions.onIntent", R"({"intent":"launch","intentId":1})")
```
The callback receives the full JSON string and must parse it with `nlohmann::json::parse(intent)`. Do not use a plain string payload such as `R"("launch")"`.

The OpenRPC fixture confirms: both `Actions.intent` and `Actions.onIntent` example results are `{"intent": "launch", "intentId": 1}` (verified in `docs/openrpc/the-spec/firebolt-open-rpc.json`).

---

## 13. CMake and Build

**Current practice:**
- C++ standard: C++17, required (`CXX_STANDARD 17`, `CXX_STANDARD_REQUIRED YES`).
- Warning flags: `-Wall -Wextra -Wpedantic` are unconditionally applied in `CMakeLists.txt`.
- New `*_impl.cpp` files are picked up automatically via `file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp json_types/*.cpp)` in `src/CMakeLists.txt`.
- New test files are picked up automatically via `file(GLOB UNIT_TESTS CONFIGURE_DEPENDS unit/*Test.cpp)` and `file(GLOB COMPONENT_TESTS CONFIGURE_DEPENDS component/*Test.cpp)`.
- Export macro: `FIREBOLTCLIENT_EXPORT` from the generated `firebolt/client_export.h`. Apply to public symbols in `include/firebolt/firebolt.h`.

**Anti-pattern:** Do not manually list sources in `src/CMakeLists.txt` — the glob handles this. Do not introduce new `CMakeLists.txt` files inside nested subdirectories under `src/` or `test/` (e.g., `src/json_types/`, `test/unit/`, `test/component/`). The top-level `src/CMakeLists.txt` and `test/CMakeLists.txt` already exist and must not be replaced.

**Formatting enforced by CI:** `clang-format` with the project's `.clang-format` (LLVM-based, column limit 120, 4-space indent, Allman braces, C++17). Running `git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror` is a required CI check. Do not bypass it.

**CI compatibility:** All build and source changes must remain compatible with the CI workflow. Do not modify build configuration in a way that passes locally but diverges from the Docker-based environment defined in `.github/workflows/ci.yml` and `.github/scripts/run-component-tests.sh`.

---

## 14. Anti-Patterns Catalogue

The following patterns are explicitly wrong for this codebase. Each entry notes where the risk originates.

| # | Anti-Pattern | Why It Is Wrong Here |
|---|---|---|
| AP-1 | Returning `std::optional<T>` instead of `Result<T>` from interface methods | Cannot carry an error code; breaks the uniform error contract used across all 13 modules |
| AP-2 | Throwing exceptions from `*_impl.cpp` method bodies | Consumers expect `Result<T>`; exceptions escape the module boundary unexpectedly |
| AP-3 | Adding `unique_ptr` or `shared_ptr` for module ownership in `FireboltAccessorImpl` | All modules are owned by value in `FireboltAccessorImpl`; smart pointers add indirection with no benefit here |
| AP-4 | Storing `IHelper` by pointer | Consistent reference storage; pointer would allow null and is not the established contract |
| AP-5 | Making `*Impl` classes copyable or movable | They hold a non-owning reference (`helper_`) and a `SubscriptionManager`; copying/moving would silently break subscription ownership tracking |
| AP-6 | Calling `IFireboltAccessor::Instance()` in unit tests | Unit tests must isolate the impl with `MockHelper`; the singleton instantiates real transport |
| AP-7 | Hardcoding JSON field names as magic strings in `*_impl.cpp` | Field names must live in `src/json_types/` only; impl code must not parse JSON directly |
| AP-8 | Adding `nlohmann::json` includes to `include/firebolt/*.h` | Public headers must not expose the JSON library as a transitive dependency |
| AP-9 | Adding logging to `*_impl.cpp` | Logging is intentionally absent in module implementations; all diagnostics go through the transport layer |
| AP-10 | Writing a new module that omits `subscribeOnStateChanged`-style subscription when the OpenRPC spec has `on*` events | Subscriptions are load-bearing API surface; omitting them silently breaks consumer event handling |
| AP-11 | Assuming `Actions.onIntent` uses a plain string trigger payload | The actual payload is a JSON-encoded object: `triggerEvent("Actions.onIntent", R"({"intent":"launch","intentId":1})")` (confirmed at `test/component/actionsGeneratedTest.cpp:62`). Always check each module's component test file and the OpenRPC fixture for the correct payload shape before writing event trigger calls |
| AP-12 | Using `#include <regex>` or other heavyweight headers without a direct use | Unnecessary includes increase compile time and leak transitive dependencies into consumers' include graphs. `-Wall -Wextra -Wpedantic` do not warn on unused includes; keep includes minimal as a discipline, not for warning suppression. `<regex>` is the canonical example of a heavyweight header with no use in this codebase |
| AP-13 | Defining a new public header without `#pragma once` | Bespoke headers require `#pragma once`; `#ifndef` guards are reserved for auto-generated output |
| AP-14 | Modifying auto-generated files by hand | Files with `// AUTO-GENERATED by fb-gen — DO NOT EDIT` must be regenerated via the generator tool |

---

## 15. Explicit Assumptions

The following items are inferred from code patterns where no explicit policy documentation existed. They are treated as policy until contradicted.

| ID | Assumption |
|---|---|
| A-1 | `FIREBOLT_LOG_*` macros come from `FireboltTransport`. Individual module impls intentionally omit logging — inferred from the absence of any logging in 12 of 13 impl files. |
| A-2 | The `using namespace Firebolt::Helpers;` pattern in `stats_impl.cpp` and `lifecycle_impl.cpp` is incidental rather than policy — inferred from its absence in the other 11 impl files. |
| A-3 | `StatsImpl`'s, `LifecycleImpl`'s, and `LocalizationImpl`'s non-`explicit` constructors are legacy remnants — confirmed at `src/stats_impl.h:30`, `src/lifecycle_impl.h:37`, `src/localization_impl.h:29`. `StatsImpl`'s non-`= default` destructor is also a remnant. All other impls use `explicit` and `= default`. |
| A-4 | Wire names for TextToSpeech events (`onWillspeak`, `onSpeechstart`, etc.) are lowercase-concatenated because the Firebolt protocol lowercases them — inferred from the pattern in `src/texttospeech_impl.cpp` and the absence of a different naming convention for other modules' events. |

---

## 16. Relationship to Existing Policy Files

This document is the single authoritative source for coding conventions and workflow rules in the `firebolt-cpp-client` repository. It supersedes `.github/copilot-instructions.md`, which has been absorbed into this document and deleted.

`CONTRIBUTING.md` governs contribution process. This document governs code shape and workflow.

---

## 17. Test Execution Commands

**Component tests (current preferred, local):**
```bash
./run-component-tests-local.sh
./run-component-tests-local.sh --skip-image-build   # reuse existing Docker image
```

**Unit tests only:**
```bash
./run-unit-tests.sh
```

Always run component tests after any API-facing change. Component tests run in Docker against mock-firebolt and are the authoritative validation gate.
