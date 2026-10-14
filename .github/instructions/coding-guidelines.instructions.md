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
- `[VERIFY]` — could not be confirmed from the repository; investigate before relying on it. Open items are collected in §18.

---

## 0. Source of Truth, Precedence, and Maintenance

When sources disagree, resolve in this order:

1. **`Firebolt_9_API_Oct1.md`** (repo root, status DRAFT) — authoritative for the API surface: method names, parameters, return types, error semantics, and whether a method is in scope for C++17 (`C++17: ✅/❌`).
2. **Merged code in this repo** — authoritative for structure and conventions.
3. **`docs/openrpc/the-spec/firebolt-open-rpc.json`** — test fixture consumed by mock-firebolt. Secondary evidence only; it can lag or diverge from the spec (see §18).
4. **This document** — a summary of 1–3; it never overrides them.

Rules:
- Only implement methods marked **Approved** with `C++17: ✅` in the spec. `Firebolt.apiVersion` and `ParentalControl.*` are *Not approved* and must not be implemented without owner approval.
- A method marked `Deprecated API version: 9.0.0` (all of `TextToSpeech`) is kept for compatibility. New speech work targets `SpeechSynthesis`.
- If code or fixture disagrees with the spec, do not copy the code's behaviour into new work. Follow the spec, and add the disagreement to §18 if it is not already there.
- The spec file name is date-stamped. If it is replaced, update every reference in this document.
- **Maintenance:** when a statement here contradicts the code, fix the statement in the same change. If you cannot tell which is right, mark it `[VERIFY]` and add it to §18 rather than guessing.
- Last verified against the repository on 2026-10-14.

---

## 1. Architecture and Module Boundaries

### 1.1 Three-Layer Architecture

The codebase has exactly three conceptual layers. Do not collapse or skip layers.

| Layer | Location | Purpose |
|---|---|---|
| Public API | `include/firebolt/<module>.h` | Pure virtual interfaces; the only surface consumers see |
| Implementation | `src/<module>_impl.h` + `src/<module>_impl.cpp` | Concrete logic; hidden from consumers |
| JSON deserialization | `src/json_types/<module>.h` | Wire format ↔ native type adapters; never exposed publicly |

**Current practice (confirmed across all existing modules):**
- There are 15 modules: accessibility, actions, advertising, device, discovery, display, lifecycle, localization, metrics, network, presentation, speechsynthesis, stats, texttospeech, videooutput.
- Every module has exactly one interface (`I<Module>`), one impl (`<Module>Impl`), and zero or more JSON type adapters. Four modules (discovery, localization, network, presentation) have no `json_types/` file; they use primitive-type helpers (`Firebolt::JSON::String`, `Boolean`, `Unsigned`) directly. `src/json_types/common.h` holds cross-module shared types and is not a module.
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
4. `unsubscribeAll()` call in the private `unsubscribeAll()` helper — **only if the module supports subscriptions** (i.e., the interface exposes any `subscribeOn*` methods).

Currently 10 modules are cleaned up there: accessibility, actions, device, lifecycle, localization, network, presentation, texttospeech, videooutput, speechsynthesis. Advertising, discovery, display, metrics, and stats have no subscriptions and are intentionally absent. When you add the first subscription to a module, add it to this helper in the same change.

Reference: `src/firebolt.cpp` (`FireboltAccessorImpl` ctor initializer list and `unsubscribeAll()`).

### 1.3 API-Facing Change Discipline

When making an API-facing change — adding or modifying an interface method, changing a return type, or adding a subscription — first check the method against `Firebolt_9_API_Oct1.md` (§0), then update all of these in a single commit:
1. `include/firebolt/<module>.h`
2. `src/<module>_impl.h` and `src/<module>_impl.cpp` (and `src/json_types/<module>.h` if the payload is a struct/enum)
3. `test/unit/<module>Test.cpp` and `test/component/<module>Test.cpp`
4. `docs/openrpc/the-spec/firebolt-open-rpc.json` — method, schema, and examples (component tests read it).
5. `test/api_test_app/apis/<module>Demo.{h,cpp}` — where a demo exists for the module. Demos currently exist for accessibility, actions, advertising, device, discovery, display, lifecycle, localization, metrics, network, presentation, stats, texttospeech; none for speechsynthesis or videooutput.

Never leave the layers out of sync after a commit. A compile-passing diff that omits a test update or fixture update is incomplete.

After the change, run `./run-unit-tests.sh`, `./run-component-tests-local.sh`, and `./lint.sh` (see §17) before pushing.

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
- TextToSpeech event names are camelCase like every other module (`"TextToSpeech.onWillSpeak"`, `"TextToSpeech.onSpeechStart"`, `"TextToSpeech.onSpeechComplete"`). Only two TextToSpeech *method* names are all-lowercase: `"TextToSpeech.listvoices"` and `"TextToSpeech.getspeechstate"`. Both must match the OpenRPC fixture exactly.

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
- Exceptions, all in files named `*GeneratedTest.cpp` (see §12): `ActionsGeneratedUTest`, `ActionsGeneratedCTest`, `VideooutputGeneratedUTest`, and `VideooutputGeneratedRuntimeCTest`. `ActionsUTest` (in `test/unit/actionsTest.cpp`) follows the standard pattern. The `*GeneratedUTest` classes do not inherit `MockBase`.

**Recommended going forward:** New manually-authored test files must follow the `<Module>UTest` / `<Module>CTest` naming.

---

## 3. Header Guards and Include Style

### 3.1 Header Guards

**Current practice:**
- Most headers (13 of 15 modules' interface and impl headers, and most `json_types/`): use `#pragma once`.
- The `actions` and `videooutput` modules use `#ifndef FIREBOLT_<NAME>_H` / `#define` / `#endif` guards in their interface, impl, and json_types headers (e.g., `include/firebolt/actions.h`, `src/actions_impl.h`, `src/json_types/videooutput.h`). These two modules also have `*GeneratedTest.cpp` files; see §12 for provenance.
- Do not mix both guards in the same file. Two files currently do: `src/json_types/actions.h` and `src/videooutput_impl.h` carry both `#pragma once` and `#ifndef`. Cleanup: keep one.

**Recommended going forward:** New headers use `#pragma once` exclusively. When editing an `actions`/`videooutput` header, keep its existing `#ifndef` guard rather than converting it.

### 3.2 Include Style

**Current practice (confirmed across all existing modules):**
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
  - `src/stats_impl.h` — `StatsImpl(Firebolt::Helpers::IHelper&)`
  - `src/lifecycle_impl.h` — `LifecycleImpl(Firebolt::Helpers::IHelper&)`
  - `src/localization_impl.h` — `LocalizationImpl(Firebolt::Helpers::IHelper&)`
- Method return types and parameter types in `*_impl.h` must exactly match those declared in the corresponding public interface header. Always use `uint32_t` (from `<cstdint>`), never the non-standard POSIX type `u_int32_t`. A type mismatch between interface and override produces an invalid override and fails to compile on non-POSIX targets.

  **Cleanup to fix:** `src/device_impl.h` — `timeInActiveState()` is declared as `u_int32_t`; must be changed to `uint32_t` to match `include/firebolt/device.h`.

### 4.2 Deleted Copy Operations

**Current practice:** Copy constructor and copy assignment operator are explicitly deleted in **all** 15 `*Impl` classes and in `FireboltAccessorImpl` in `src/firebolt.cpp`. This is mandatory.

### 4.3 Move Operations

All `*Impl` classes and `FireboltAccessorImpl` must explicitly delete the move constructor and move assignment operator, placed immediately after the deleted copy operations:

```cpp
ClassName(ClassName&&) = delete;
ClassName& operator=(ClassName&&) = delete;
```

**Rationale:** These classes hold a non-reassignable reference member (`helper_`) and, where applicable, a `SubscriptionManager`. A compiler-generated move would leave the source object with a dangling reference or corrupted subscription state. Explicit deletion makes the intent clear and prevents accidental moves at call sites.

**Cleanup to fix:** None of the 15 `*Impl` classes nor `FireboltAccessorImpl` currently deletes move operations. Add the two deleted move declarations to every `src/*_impl.h` and to `FireboltAccessorImpl` in `src/firebolt.cpp`. New and modified `*Impl` classes must include them.

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

**Current practice (without exception across all existing modules):**
- Every method in `I<Module>` returns `Result<T>` or `Result<void>`.
- `Result<void>` is used for methods that send a command and carry no return value (e.g., `Metrics.ready()`, `Lifecycle.close()`).
- Callers check the result with boolean conversion (`if (result)`) or dereference after assertion (`*result`).

**Anti-pattern:** Do not throw exceptions from public interface methods. Do not return bare `T` where failure is possible. Do not use `std::optional<T>` as a substitute for `Result<T>` — `optional` cannot carry an error code.

**Constraint (no exceptions across the library boundary):** the library is consumed by a porting layer that may be built without C++ exceptions, so no exception may propagate out of any public method. Public signatures take plain types (`std::string`, `uint32_t`, `std::optional<...>`), not types whose construction can throw. For any entry point that parses caller-supplied text, catch the parse failure inside the impl and return `Error::InvalidParams`. Reference: `ActionsImpl::start()` in `src/actions_impl.cpp`. `[VERIFY]` the original design decision (recorded for RDKEMW-26413) with the library owners if you need to relax it.

### 5.2 Error Propagation in Implementations

**Current practice:** Implementations propagate errors by returning the result of `helper_.get<>()`, `helper_.invoke()`, or `subscriptionManager_.subscribe<>()` directly. The only `try`/`catch` in any `*_impl.cpp` is in `ActionsImpl::start()`, which converts a `nlohmann::json::parse_error` on the caller's intent string into `Error::InvalidParams`.

**Anti-pattern:** Do not add `try-catch` blocks in `*_impl.cpp` for errors the helper/transport already handles. A `try-catch` is permitted only to convert a failure while parsing **caller-supplied** input into `Result<T>{Error::InvalidParams}`. Do not swallow errors silently.

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

### 6.4 Units in JSON Adapters (Stats)

**Current practice:** `Stats.memoryUsage` carries `userMemoryUsed`, `userMemoryLimit`, `gpuMemoryUsed`, `gpuMemoryLimit` in **bytes**, matching `Firebolt_9_API_Oct1.md` and the fixture. `src/json_types/stats.h` reads them as `uint64_t` with no conversion, and `Stats::MemoryInfo` exposes `uint64_t`. Tests compare against the fixture values directly.

`[VERIFY]` The spec types these as 32-bit `unsigned`; the code uses `uint64_t`. See §18.

**Anti-pattern:** Do not add unit conversion (e.g., ×1024) inside `fromJson()` without a matching change to the public API type, tests, fixture, and a spec confirmation.

### 6.5 Raw JSON Payloads (Actions)

`Actions::Intent::intent` holds the intent as raw JSON **text**. `JsonData::JsonValue::fromJson()` stores `json["intent"].dump()`. `nlohmann::json` never appears in a public header. Because `dump()` may reorder object keys, tests must compare structurally (`nlohmann::json::parse(result->intent) == expected`), never by raw string equality. See §12.4.

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

**Current practice:** Used for methods that return `Result<void>`. Parameters are constructed as `nlohmann::json` before the call. Optional parameters are conditionally added. Request parameter names are written as literals in the impl and must match the spec's parameter names. Response parsing never happens in the impl; it lives in `json_types/`.

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

**Current practice (the fixture style below is the most common, but not universal):**
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

Variations that exist today: `TextToSpeechCTest` declares `cv`/`mtx`/`eventReceived` as locals inside each test; `SpeechSynthesisCTest` uses members named `mtx_`, `cv_`, `eventReceived_`; `MetricsCTest::SetUp()` is empty. Keep the style of the file you are editing.

**Current practice — triggering events:**
- String payload: `triggerEvent("Module.onEvent", R"("string_value")")` — note outer double-quotes in JSON
- Object payload: `triggerEvent("Module.onEvent", R"({"field": value})")`
- Scalar events: two forms are used today. Wrapped: `R"({"value": true})"` (Accessibility, Network, Presentation, Localization, `Device.onDolbyAtmosExperienceAvailableChanged`). Bare: `R"("Living Room TV")"` (`Device.onNameChanged`) and `R"("hdcp1.4")"` (VideoOutput). `[VERIFY]` which form is canonical for mock-firebolt (§18). Copy the form used by the nearest existing test for the same payload type.
- For `Actions.onIntent`, the payload is an object whose `intent` member is itself a JSON **object**: `R"({"intent":{"action":"pre-load",...},"intentId":0})"`. The callback receives `Actions::Intent` with `intent` as raw JSON text. See §12.4 for the full contract.

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
- Invoke-style (`Result<void>`) method: call `mockInvoke("Module.method")`, then the impl method, then `ASSERT_TRUE(result)`.
- Subscribe test: call `mockSubscribe("Module.onEvent")`, subscribe, assert `ASSERT_TRUE(result)`, then call `unsubscribe` and assert success.
- Enum validation: `validate_enum("EnumName", Firebolt::<Module>::JsonData::<EnumName>Enum)` checks the fixture's schema against the code's enum map.

**Anti-pattern:** Do not test `*Impl` via `IFireboltAccessor::Instance()` in unit tests. Unit tests must isolate the impl with a mock helper, not the full singleton.

**Exception for `*GeneratedTest.cpp` unit tests:** `test/unit/actionsGeneratedTest.cpp` and `test/unit/videooutputGeneratedTest.cpp` instantiate `::testing::NiceMock<MockHelper>` directly without inheriting `MockBase`, and use `EXPECT_CALL(mockHelper, getJson(...))` rather than the `mock()` / `mock_with_response()` wrappers. Do not replicate this in new bespoke test files.

Bespoke tests (`discoveryTest.cpp`, `speechSynthesisTest.cpp`, `textToSpeechTest.cpp`) do use `EXPECT_CALL(mockHelper, getJson/invoke(...))` alongside `MockBase` when they need to assert the exact request payload. That is acceptable for payload assertions.

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

**Component test log expectations:** mock-firebolt prints lines of the form `<Event> validation error for <Event> with <payload>, err: [...]` when a negative-path test injects an invalid payload. This is expected: the mock rejects the payload (event validation is enabled in `.github/mock-firebolt/config.json`), so the client never receives it. Do not treat these lines as failures and do not suppress them by weakening the test.

Negative tests must verify runtime behaviour — specifically that callbacks are not delivered when the payload is invalid. A test that merely asserts the code compiles with an invalid type is insufficient. Always pair with `verifyEventNotReceived`. Do not relax or remove a negative test because it produces validation-error log lines.

### 10.3 Pairing Rule

**Current practice (confirmed across all existing modules):** Every module has both a unit test file and a component test file. When adding a new module or method:
- Add unit tests in `test/unit/<module>Test.cpp`
- Add component tests in `test/component/<module>Test.cpp`
- Both test files must cover all public API methods
- Each getter/property method must have at minimum: one happy-path test and one bad-response negative test. This is required for new and modified code; existing coverage is not uniformly complete `[VERIFY]`.

### 10.4 Expected Values from OpenRPC

**Current practice:** Both unit and component tests derive expected values from `jsonEngine.get_value("Module.method")` (the first example in the OpenRPC fixture). Do not hardcode values that duplicate the fixture unless the value requires a type conversion (e.g., enum comparison using `static_cast<int>`).

Exception: `test/component/actionsGeneratedTest.cpp` hardcodes the intent payload (`{"action":"pre-load",...}`, `intentId` 0) copied from the fixture's `Actions.intent` / `Actions.onIntent` example. If the fixture example changes, update that test in the same commit.

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

### 11.3 Event Payload Validation in mock-firebolt

- `.github/mock-firebolt/config.json` enables validation of `method`, `params`, `response`, and `events`. A `triggerEvent` payload that fails validation is dropped, and the test fails with `Did not receive event within timeout`.
- Observed rule (recorded 2026-10-05 for the TextToSpeech events): for an event **with** `x-subscriber-for`, mock-firebolt validates the payload against the linked getter's result schema, so the event's own `result` may be `{"type": "null"}`. For a pure notifier **without** `x-subscriber-for` (all `TextToSpeech.on*`, `SpeechSynthesis.on*`), it validates against the event's own `result` schema, which must therefore describe the real payload (e.g. `$ref: SpeechIdEvent`).
- `[VERIFY]` `Lifecycle2.onStateChanged` is a pure notifier whose fixture `result` is `{"type": "null"}`, yet its component tests deliver payloads successfully. The rule above is therefore incomplete; confirm the exact behaviour against the pinned mock-firebolt revision before relying on it.
- The mock-firebolt branch and commit are pinned in two places that must stay identical: `MOCK_BRANCH` / `MOCK_SHA1SUM` in `.github/workflows/tests-trigger.yml`, and the same two variables in `run-component-tests-local.sh`. Update both together.
- With `--protocol legacy`, `.github/scripts/run-component-tests.sh` switches to `*--legacy.json` variants of the mock config and the OpenRPC fixture. `[VERIFY]` those variants exist in the repo before relying on the legacy path.

---

## 12. Possibly Generated vs Bespoke Code

### 12.1 Possibly Generated Files (Provenance Unconfirmed)

No file in this repository carries an `AUTO-GENERATED` / `DO NOT EDIT` banner, and neither `firebolt-sdk-gen` nor `sync-plan-checklist.sh` exists in the repo or in the sibling checkouts under `RDKE/`. An earlier version of this document declared `actions` files generator-owned; that cannot be confirmed.

Two modules differ stylistically from the rest and are *possibly* generator-originated: `actions` and `videooutput`. They use `#ifndef` guards (§3.1), their tests are named `*GeneratedTest.cpp` (§2.7, §10.1), and their JSON adapters use a `*Json` / `*Value` naming style. Files:
- `include/firebolt/actions.h`, `src/actions_impl.{h,cpp}`, `src/json_types/actions.h`, `test/unit/actionsGeneratedTest.cpp`, `test/component/actionsGeneratedTest.cpp`
- `include/firebolt/videooutput.h`, `src/videooutput_impl.{h,cpp}`, `src/json_types/videooutput.h`, `test/unit/videooutputGeneratedTest.cpp`, `test/component/videooutputGeneratedTest.cpp`

These files have been hand-edited in this repository (for example `ActionsImpl::start()` and `JsonData::JsonValue` for RDKEMW-26413), so "never hand-edit" is not the practice.

### 12.2 Editing These Files

- Hand edits are allowed. Keep each file's existing style (guards, naming) so a future generator run produces a small diff.
- Do not perform large structural rewrites of these files until provenance is confirmed (§18, O-9).
- If a generator is later identified, record its location, command, and ownership here and restore a DO-NOT-EDIT rule only for files that actually carry a banner.

### 12.3 Structure of New Modules

New modules follow the structure of the `#pragma once` modules (for example `speechsynthesis`, `device`), not the `actions`/`videooutput` style, unless a generator is confirmed to own the module.

### 12.4 Actions Module API Contract

Spec: `Firebolt_9_API_Oct1.md` #5 `start` and #6 `intent` / `onIntent` (API 9.0.0). `intentId` is an unsigned, monotonic value; `intent` is a JSON document.

- `IActions::intent()` is a getter with no parameters. It returns `Result<Actions::Intent>`, where `struct Intent { std::string intent; uint32_t intentId{0}; }`. `intent` holds **raw JSON text**; it is not a plain scalar and not an `nlohmann::json`.
- `subscribeOnIntent(std::function<void(const Intent&)>&&)` delivers the same struct. `subscribeOnIntentChanged` is a default alias that forwards to it.
- `start(const std::string& intent, std::optional<std::string> handlerAppId = std::nullopt)` returns `Result<void>`. A malformed `intent` string returns `Error::InvalidParams`; it must never throw (§5.1).
- Wire shape (fixture): `{"intent": {"action": "pre-load", ...}, "intentId": 0}` — `intent` is a JSON object on the wire.
- Tests compare structurally, not by string equality (`dump()` may reorder keys). Reference: `test/component/actionsGeneratedTest.cpp`:
```cpp
auto result = Firebolt::IFireboltAccessor::Instance().ActionsInterface().intent();
ASSERT_TRUE(result) << toError(result);
EXPECT_EQ(nlohmann::json::parse(result->intent), expectedIntent);
EXPECT_EQ(result->intentId, 0U);
```
Do not add `nlohmann` types to `include/firebolt/actions.h`, and do not compare `intent` text with `==`.

---

## 13. CMake and Build

**Current practice:**
- C++ standard: C++17, required (`CXX_STANDARD 17`, `CXX_STANDARD_REQUIRED YES`).
- Warning flags: `-Wall -Wextra -Wpedantic` are unconditionally applied in `CMakeLists.txt`.
- New `*_impl.cpp` files are picked up automatically via `file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp json_types/*.cpp)` in `src/CMakeLists.txt`.
- New test files are picked up automatically via `file(GLOB UNIT_TESTS CONFIGURE_DEPENDS unit/*Test.cpp)` and `file(GLOB COMPONENT_TESTS CONFIGURE_DEPENDS component/*Test.cpp)`.
- Export macro: `FIREBOLTCLIENT_EXPORT` from the generated `firebolt/client_export.h`; it is applied to `IFireboltAccessor` in `include/firebolt/firebolt.h`. Symbol visibility is `hidden` unless `ENABLE_TESTS` is on, so only exported symbols are usable from outside the library in a normal build. Do not reference `*Impl` classes from the demo app or other consumers.
- Transport dependency: `.transport.version` pins the expected `firebolt-cpp-transport`; the root `CMakeLists.txt` compares major versions only and reports a mismatch via `message(STATUS ...)`.

**Anti-pattern:** Do not manually list sources in `src/CMakeLists.txt` — the glob handles this. Do not introduce new `CMakeLists.txt` files inside nested subdirectories under `src/` or `test/` (e.g., `src/json_types/`, `test/unit/`, `test/component/`). The top-level `src/CMakeLists.txt` and `test/CMakeLists.txt` already exist and must not be replaced.

**Formatting enforced by CI:** `clang-format` with the project's `.clang-format` (LLVM-based, column limit 120, 4-space indent, Allman braces, C++17). The `format_check` job in `.github/workflows/tests-trigger.yml` runs `git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror`. Do not bypass it. Locally use `./lint.sh` (`--fix` to reformat); see §17 for a caveat that can make it check zero files.

**CI compatibility:** All build and source changes must remain compatible with the CI workflows. Do not modify build configuration in a way that passes locally but diverges from the Docker-based environment defined by `.github/Dockerfile`, `.github/workflows/tests-trigger.yml` (jobs: `build_docker`, `format_check`, `build_project`, `coverage_gate`, `update_baseline`), `.github/workflows/L1-tests.yml` (unit tests and coverage), `.github/workflows/L2-tests.yml` (component tests and `api_test_app`), and `.github/scripts/run-component-tests.sh`.\n\n**Coverage gate:** `coverage_gate` compares unit and component coverage against a baseline (`.github/scripts/compare_coverage.py`). New or changed code must come with tests (\u00a710.3) or the gate can fail.

---

## 14. Anti-Patterns Catalogue

The following patterns are explicitly wrong for this codebase. Each entry notes where the risk originates.

| # | Anti-Pattern | Why It Is Wrong Here |
|---|---|---|
| AP-1 | Returning `std::optional<T>` instead of `Result<T>` from interface methods | Cannot carry an error code; breaks the uniform error contract used across all modules |
| AP-2 | Throwing exceptions from `*_impl.cpp` method bodies | Consumers expect `Result<T>`; exceptions escape the module boundary unexpectedly |
| AP-3 | Adding `unique_ptr` or `shared_ptr` for module ownership in `FireboltAccessorImpl` | All modules are owned by value in `FireboltAccessorImpl`; smart pointers add indirection with no benefit here |
| AP-4 | Storing `IHelper` by pointer | Consistent reference storage; pointer would allow null and is not the established contract |
| AP-5 | Making `*Impl` classes copyable or movable | They hold a non-owning reference (`helper_`) and a `SubscriptionManager`; copying/moving would silently break subscription ownership tracking |
| AP-6 | Calling `IFireboltAccessor::Instance()` in unit tests | Unit tests must isolate the impl with `MockHelper`; the singleton instantiates real transport |
| AP-7 | Parsing **response** JSON in `*_impl.cpp`, or hardcoding response field names there | Response deserialization lives in `src/json_types/` only. Building **request** parameter objects with literal names in the impl is normal and required (names must match the spec). The sole parse in an impl is `ActionsImpl::start()` validating caller input (§5.2) |
| AP-8 | Adding `nlohmann::json` includes to `include/firebolt/*.h` | Public headers must not expose the JSON library as a transitive dependency |
| AP-9 | Adding logging to `*_impl.cpp` | Logging is intentionally absent in module implementations; all diagnostics go through the transport layer |
| AP-10 | Writing a new module that omits `subscribeOnStateChanged`-style subscription when the OpenRPC spec has `on*` events | Subscriptions are load-bearing API surface; omitting them silently breaks consumer event handling |
| AP-11 | Copying an event payload from another module without checking its shape | Shapes differ: wrapped scalars (`{"value": true}`), bare scalars (`"hdcp1.4"`), objects (`{"speechid": 1}`), and `Actions.onIntent` (`{"intent": {...}, "intentId": 0}`, with `intent` an object). Check the module's component test and the fixture before writing `triggerEvent` calls (§8.2) |
| AP-12 | Using `#include <regex>` or other heavyweight headers without a direct use | Unnecessary includes increase compile time and leak transitive dependencies into consumers' include graphs. `-Wall -Wextra -Wpedantic` do not warn on unused includes; keep includes minimal as a discipline, not for warning suppression. `<regex>` is the canonical example of a heavyweight header with no use in this codebase |
| AP-13 | Defining a new header without `#pragma once` | New headers use `#pragma once`; `#ifndef` guards exist only in the `actions` and `videooutput` modules (§3.1) |
| AP-14 | Large structural rewrites of `actions` / `videooutput` files before provenance is confirmed | They may be generator-originated (§12.1); small targeted edits are fine and have precedent |
| AP-15 | Treating `./lint.sh` printing "Formatting OK" as proof of a clean format on macOS/Docker | The script pipes `git ls-files` into `xargs` without `pipefail`; if Git rejects the repo as "dubious ownership", zero files are checked. Confirm the file count (§17) |
| AP-16 | Regenerating `docs/openrpc/the-spec/*.json` (`npm run the-spec`) and committing without diffing | The per-module sources under `docs/openrpc/openrpc/` use different TextToSpeech event names (`onWillspeak`) than the merged fixture and client (`onWillSpeak`). A blind regeneration can break component tests (§18, O-5) |

---

## 15. Explicit Assumptions

The following items are inferred from code patterns where no explicit policy documentation existed. They are treated as policy until contradicted.

| ID | Assumption |
|---|---|
| A-1 | `FIREBOLT_LOG_*` macros come from `FireboltTransport`. Individual module impls intentionally omit logging — inferred from the absence of any logging in all 15 `*_impl.cpp` files. |
| A-2 | The `using namespace Firebolt::Helpers;` pattern in `stats_impl.cpp` and `lifecycle_impl.cpp` is incidental rather than policy — inferred from its absence in the other 13 impl files. |
| A-3 | The non-`explicit` constructors of `StatsImpl`, `LifecycleImpl`, and `LocalizationImpl`, and the out-of-line empty destructors of `StatsImpl` and `LifecycleImpl`, are legacy remnants. All other impls use `explicit` and `= default`. Locations: `src/stats_impl.h`, `src/lifecycle_impl.h`, `src/localization_impl.h`. |
| A-4 | TextToSpeech event wire names are camelCase (`onWillSpeak`, `onSpeechStart`, ...) as used in `src/texttospeech_impl.cpp` and `the-spec/firebolt-open-rpc.json`. Commit `89cb7fc` ("Fix TTS event wire names to use camelCase") corrected earlier lowercase names. The per-module source `docs/openrpc/openrpc/text_to_speech.json` still has lowercase names — see §18, O-5. |

---

## 16. Relationship to Existing Policy Files

This document is the single authoritative source for coding conventions and workflow rules in the `firebolt-cpp-client` repository. It supersedes `.github/copilot-instructions.md`, which has been absorbed into this document and deleted.

`CONTRIBUTING.md` governs contribution process. This document governs code shape and workflow.

---

## 17. Test Execution Commands

```bash
./run-unit-tests.sh                      # builds build-dev (+tests), runs utApp
./run-unit-tests.sh --unit-filter "ActionsUTest.*"
./run-component-tests-local.sh           # Docker: builds ctApp in build-docker, runs against mock-firebolt
./run-component-tests-local.sh --skip-image-build   # reuse existing Docker image
./lint.sh                                # clang-format check; --fix to reformat; SKIP_DOCKER=1 or --local for host clang-format
```

Always run component tests after any API-facing change. Component tests run in Docker against mock-firebolt and are the authoritative validation gate. Run unit tests, component tests, and lint before pushing.

### Known environment pitfalls (macOS + Docker Desktop)

Verified on 2026-10-14. These are environment issues, not code defects; do not "fix" them by editing source.

- **`./run-unit-tests.sh` fails with "CMakeCache.txt directory is different than ... /workspace/build-dev"** when `build-dev` was previously built inside Docker (its cache records `/workspace`). Use `./run-unit-tests.sh --clean` (removes `build-dev`) or build in a fresh directory.
- **`./run-component-tests-local.sh` fails with "detected dubious ownership in repository at '/mock-host'"** when it clones/updates mock-firebolt in the container. It also deletes and recreates `build-docker` on every run.
- **`./lint.sh` can print "Formatting OK" while checking nothing.** The container's Git prints "dubious ownership ... '/workspace'", `git ls-files` returns nothing useful, and the unguarded pipe lets `xargs clang-format` succeed on empty input. Observed: 4 lines vs. 126 tracked `*.cpp`/`*.h` files once the repo is trusted. Verify the real check with:
```bash
docker run --rm --user "$(id -u):$(id -g)" -e GIT_CONFIG_COUNT=1 -e GIT_CONFIG_KEY_0=safe.directory -e GIT_CONFIG_VALUE_0=/workspace \
  -v "$PWD:/workspace" -w /workspace firebolt-client-ci:local \
  bash -c "git ls-files -- '*.cpp' '*.h' | xargs clang-format --dry-run --Werror"
```
- **Running components manually** inside `firebolt-client-ci:local` additionally needs `-e PATH=/usr/local/nvm/versions/node/v24.11.0/bin:$PATH` and `-e npm_config_cache=/tmp/npm-cache` for `npm ci` in mock-firebolt (the image's default `PATH` has no `node`/`npm`, and `/.npm` is root-owned), plus the same `GIT_CONFIG_*` safe-directory variables for `/workspace` and `/mock`.

Proposed script fixes (not yet applied): add `set -o pipefail` and a safe-directory setting to `lint.sh`; add the same safe-directory setting and an `npm` cache/`PATH` to `run-component-tests-local.sh`.

---

## 18. Open Items Requiring Investigation

Each item is unverified or unresolved. Do not "fix" these by assumption; confirm with the owner or evidence, then update this section and the relevant rule above.

| ID | Item | Evidence | Needed |
|---|---|---|---|
| O-1 | Spec methods marked `C++17: ✅` that the client does not implement | Device: `manufacturer`, `brandName`, `modelId`. Display: `connected` / `onConnectedChanged`. Network: `interfaces`, `interfaceStatus`/`onInterfaceStatusChanged`, `interfaceConfig`/`onInterfaceConfigChanged`, `interfaceStatistics`, `interfaceDetails`/`onInterfaceDetailsChanged` (none in `include/firebolt/*.h`) | Confirm roadmap and ticket owner; these are gaps, not intentional exclusions |
| O-2 | Client methods that are not in the spec | `Device.setOsName`, `Device.setOsVersion`; `Discovery.watchedV2`; extra optional `TextToSpeech.speak` parameters (`callSign`, `language`, `voice`, `volume`, `rate`, `pitch`) appear in the fixture but spec #73 lists only `text` | Confirm they are intended extensions or align to the spec |
| O-3 | `Discovery.watched` shape | Spec #22 returns None. `watched()` returns `Result<bool>` (fixture agrees); `watchedV2()` returns `Result<void>` but invokes wire method `"Discovery.watched"` (unit tests assert this), while the fixture defines a separate `Discovery.watchedV2` returning null | Decide the canonical wire method and return type |
| O-4 | Stats type width | Spec #72 says 32-bit `unsigned`; `Stats::MemoryInfo` and `json_types/stats.h` use `uint64_t` (bytes, no conversion) | Confirm `uint64_t` is intended; update spec or code |
| O-5 | TextToSpeech event names diverge between OpenRPC sources | Per-module `docs/openrpc/openrpc/text_to_speech.json` uses `onWillspeak`/`onSpeechstart`/... (`x-event`); tracked merged fixture `docs/openrpc/the-spec/firebolt-open-rpc.json` and the client use `onWillSpeak`/`onSpeechStart`/... Merged fixture appears to be edited directly | Define the official regeneration process (`npm run the-spec` in `docs/openrpc`), fix the sources, or document that the merged file is hand-maintained |
| O-6 | Event validation rule in mock-firebolt (§11.3) is incomplete | `Lifecycle2.onStateChanged` has a `null` result schema and no `x-subscriber-for`, yet its component tests deliver payloads | Confirm validation behaviour at the pinned mock revision |
| O-7 | Canonical scalar event payload form | Both `{"value": x}` and bare `x` are used in component tests | Confirm what the mock/gateway emit on the real wire |
| O-8 | Spec error semantics | Spec lists codes (-32601, -40300, -32602, -32100, -50100) and unassigned `-xxxxx` for *App state invalid*, *Speech synthesis engine error*, *Wrong device class* | Verify how these map to `Firebolt::Error` (defined in the transport dependency, not in this repo) |
| O-9 | Provenance of `actions` / `videooutput` files | No generator banner; `firebolt-sdk-gen` and `sync-plan-checklist.sh` not found (§12.1) | Identify the generator, if any, and its ownership rules |
| O-10 | Lifecycle state set | Spec #31 lists six states with no `unloaded`; the current enum matches the spec. A backend `unloaded` response was handled on a separate branch (RDKEMW-26354) | Confirm whether `unloaded` is a real wire value and update the spec/enum together |
| O-11 | Open cleanups from §4 and §3.1 | Deleted moves everywhere; `explicit` on three impls; empty destructors on two; `u_int32_t` in `src/device_impl.h`; `friend class ::LifecycleTest` in `src/lifecycle_impl.h`; `using namespace` in two `.cpp` files; double guards in `src/json_types/actions.h` and `src/videooutput_impl.h` | Schedule a single cleanup change |
| O-12 | Script defects in §17 | `lint.sh` no `pipefail`/safe-directory; `run-component-tests-local.sh` safe-directory, `npm`/`PATH`, and cache handling. Whether CI's `format_check` is exposed to the same ownership problem is unverified | Fix scripts; confirm CI behaviour |
