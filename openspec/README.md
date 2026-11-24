# firebolt-cpp-client specifications

These files define the public API and the behaviour of the Firebolt C++ client. They are the source of truth: code and tests follow them. `Firebolt9SpecNov.md` is the upstream Firebolt 9 definition; where this client deliberately differs from it, the spec says so.

## Layout

```
openspec/specs/
  common/        shared types, error codes, and rules that apply to every module
  <module>/      one folder per Firebolt module (lower case, words joined by "-")
    api.yaml     the contract: methods, events, types, constraints, examples
    spec.md      behaviour the contract cannot express (only when there is some)
```

| Question | Where the answer lives |
|---|---|
| What is sent on the wire, with which names, types and limits? | `api.yaml` |
| What does the client do beyond that (checks, edge cases, ordering)? | `spec.md` of the module |
| What holds for every module (errors, validation, events)? | `common/spec.md` |

A fact is written once. If the schema already says it (a field is required, a value is in a set), no `spec.md` repeats it.

`spec.md` follows the OpenSpec format: `### Requirement:` with SHALL, and `#### Scenario:` with WHEN/THEN. Check with `openspec validate --specs --strict`.

## api.yaml field guide

```yaml
module: Device                 # wire module; a method is sent as "<module>.<method>"
description: ...
cpp: {module: Lifecycle}       # optional: C++ module name, only when it differs from `module`
deprecated: {in: "9.0.0", replacement: SpeechSynthesis}   # optional
types:    { <Name>: ... }
methods:  { <name>: ... }      # calls and getters
events:   { <name>: ... }      # subscriptions
```

**types**: a type is exactly one of:

| Form | Meaning |
|---|---|
| `{type: string, pattern: ..., description: ...}` | a primitive with limits |
| `{enum: {<wire value>: <C++ enumerator>}}` | an enum (C++ `enum class`) |
| `{fields: {<name>: {type: ..., optional: true}}}` | an object (C++ struct) |

**Primitive types**: `string`, `boolean`, `double` (64-bit float), `unsigned` (32-bit unsigned integer), `unsigned64` (64-bit; `Stats` only), `integer`, `json` (any JSON value). `array<T>` is a list and `map<T>` an object whose values are `T`. A type from another module is written `Module.Type`.

**Limits** on a param or field: `values` (one of a fixed set, not an enum), `minimum` / `maximum` (inclusive), `minLength`, `pattern`, `format`. Formats are defined in `common/spec.md`.

**Required by default.** Params and fields are required unless marked `optional: true`.

**methods**: `summary`, `params`, `result` (a type; omitted means no value), `errors` (Firebolt 9 specific codes from `common/api.yaml`), `examples` (`{params, result}`: a sample request and its response, for tests; omitted when there are no params and no result), `extension: true` (not in Firebolt 9), `cpp_name` / `cpp_result` (only where the C++ name or result type differs from the rules below). A field can also carry `cpp_name`.

**events**: `changes: <getter>` (the payload is that getter's result, and it uses the getter's errors and example) or `payload: <type>` with `examples: [{payload: ...}]`, plus an optional `summary`. The transport adds the `listen` param; it is not declared.

## C++ mapping

| Wire | C++ |
|---|---|
| module `Foo` | header `firebolt/foo.h` (lower case), interface `Firebolt::Foo::IFoo`, reached with `IFireboltAccessor::FooInterface()` |
| method `name` | `name(...)`, same spelling |
| event `onName` | `subscribeOnName(callback)`; `unsubscribe(id)` and `unsubscribeAll()` exist on every module with events |
| params, in listed order | arguments; `optional: true` becomes `std::optional<T>` |
| result | `Result<T>`; no result is `Result<void>` |
| `string` / `boolean` / `double` / `unsigned` / `unsigned64` | `std::string` / `bool` / `double` / `uint32_t` / `uint64_t` |
| `json` | `std::string` holding serialized JSON |
| `map<string>` | `std::map<std::string, std::string>` |
| `array<T>` | `std::vector<T>` |
| object type | `struct` of the same name with the same field names |
| enum type | `enum class` of the same name; each wire value (a string or a number) maps to the listed enumerator |

Other signature details (`const`, `[[nodiscard]]`) follow `.github/instructions/coding-guidelines.instructions.md`.

## Changing an API

1. Edit `api.yaml` and, if behaviour changes, `spec.md`, in the same change as the code and tests.
2. Run `openspec validate --specs --strict`.
3. Anything that is not clear or may be a bug goes to `OPEN_QUESTIONS.md`, not into a requirement.
