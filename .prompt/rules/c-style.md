---
name: c-style
description: C coding/filename style preference. Path-scoped to source and header files.
paths:
  - "src/**/*.c"
  - "include/**/*.h"
---

# C style guide

A generic C23 style and project-layout guide, distilled from several in-house rule sets plus common community practice (Linux kernel style, the NASA/JPL "Power of Ten" safety-critical rules, and modern C23 idiom). Adapt thresholds to project scale; keep the golden rules fixed.

## Golden rules

1. **One function per translation unit.** Each `.c` file implements exactly one function, named after it. Public or private does not matter — no file grows into a dumping ground of unrelated helpers.
2. **Private symbols get a trailing underscore**, on both the function name and its file (`thing_step_.c` → `thing_step_`). Public symbols and files carry no trailing underscore.
3. **Public API is prefixed with a short project code, at most two letters** (`<pr>_<verb>` or `<pr>_<module>_<verb>`), so the symbol is unambiguous at the call site without needing the header open. The same short code prefixes public types and constants too (`<PR>_ERROR_*`, `<PR>_OPT_TYPE_*`). Spell the project name out in full only for the one core handle type (`<project>_t`) — everything else uses the short code.
4. **Every module owns its own build file** listing that module's sources. The root build file only wires modules together (`subdir(...)` / equivalent) — it never enumerates every source path itself.
5. **C23 only, no non-standard extensions** unless a compiler-specific feature is explicitly justified and isolated behind a macro.
6. **No emoji** in source, comments, or docs. Plain Unicode marks are fine where a project already uses them for CLI/log glyphs.

## Directory layout

```text
build-file                    # project root: options, deps, subdir(...) only
include/
  build-file
  <project>/
    <project>.h               # umbrella header: core + sibling public headers
    <module>.h                # one public header per top-level module
src/
  build-file
  <project>/
    build-file                 # subdir per module + top-level glue only
    <project>_private.h        # cross-module private types, if any
    <module>/
      private.h                # module-private types/decls (optional)
      build-file                # this module's source list
      unit/
        <verb>.c                # public: <project>_<module>_<verb>
        <verb>_.c                # private: <project>_<module>_<verb>_
bin/
  build-file
  <app>/…                     # executables/CLIs, thin over the library
tests/
  build-file
  <module>/…
```

### When a feature earns its own top-level module

Create `src/<project>/<module>/` + `include/<project>/<module>.h` when the feature:

- has a clear, self-contained domain,
- is or will be consumed by more than one caller/surface, and
- would otherwise mix unrelated concerns inside an existing module.

Prefer a flat top-level module over nesting. Nest a sub-module only when a single module's `unit/` directory has grown large *and* the sub-domain is still one coherent concern — never nest a module that other siblings also depend on.

### Dependency direction

Draw the module dependency graph before adding an `#include`. Lower-level / engine modules must not call back into higher-level / host modules; keep that direction one-way and enforce it by not exposing the higher module's headers to the lower one.

## Naming

`<pr>` is the project's short code: at most two letters, chosen once and used everywhere a public identifier needs a namespace (e.g. `throttle` → `th`, `beacon` → `bc`). Modules may still be spelled out in full inside the identifier — only the project part gets abbreviated.

| Kind                  | Symbol                                                      | File                                |
|-----------------------|-------------------------------------------------------------|-------------------------------------|
| Public function       | `<pr>_<module>_<verb>` (or `<pr>_<verb>` for a core module) | `unit/<verb>.c`                     |
| Private function      | `<pr>_<module>_<verb>_`                                     | `unit/<verb>_.c`                    |
| Struct / type         | `<pr>_<module>_<noun>`                                      | declared in module or public header |
| Constant / enumerator | `<PR>_<NOUN>_<VALUE>` (upper snake case)                    | declared in public header           |
| Core handle type      | `<project>_t` (full project name, exception to the rule)    | umbrella header                     |
| Module-private header | —                                                           | `<module>/private.h`                |
| Public header         | types + public decls                                        | `include/<project>/<module>.h`      |
| Umbrella header       | re-exports sibling public headers                           | `include/<project>/<project>.h`     |

Example, project `throttle` (`<pr>` = `th`):

```c
typedef struct throttle throttle_t;       // core handle: full name
typedef struct th_option th_option_t;     // everything else: short code
typedef enum th_error_code {
  TH_ERROR_UNKNOWN_OPTION = 1,            // constants: short code, upper snake
  TH_ERROR_MISSING_VALUE,
} th_error_code_t;

throttle_t *th_create(const th_settings_t *settings);
void        th_destroy(throttle_t *t);
bool        th_has(const throttle_t *t, const char *key);
```

- Files and identifiers: lowercase `snake_case`.
- Names are self-documenting — spell out what a thing is or does in full words rather than abbreviating, except for the project code itself.
- A module's constructor/destructor pair (`_create` / `_destroy`) lives in the module's top-level `.c`, not under `unit/`.

## Includes

- Public headers never include a `private.h` or the project-wide private header.
- The umbrella header may include sibling public headers, after any shared types those headers need.
- Units include their own `private.h` and, if needed, the project-wide private header — never another module's private header.
- Prefer including a sibling module's *public* header over reaching into its internals.

## Documentation

- Doxygen (or equivalent) comments only on **public** headers, kept short: what it does, ownership/lifetime of any pointer arguments, and failure modes. Skip Doxygen on private declarations.
- Source-level comments are reserved for non-obvious control flow, a workaround for a specific bug, or an invariant that isn't visible from the code itself — not for restating what a well-named function does.

## Language and safety practices

Pragmatic subset of the NASA/JPL "Power of Ten" rules, adapted for application code rather than flight software — apply the spirit, not the letter, where it would otherwise block ordinary development:

- **Bound every loop.** An unbounded loop needs an explicit, commented justification (e.g. a genuine event loop) rather than being the default.
- **Check every return value** that can signal failure; callers decide explicitly whether to propagate, retry, or abort. Callees validate the parameters callers give them at the boundary.
- **Prefer stack allocation.** Reserve heap allocation for data whose size or lifetime isn't known at compile time. Avoid allocating after a component's initialization phase where the workload allows it.
- **Keep functions short** — a function that no longer fits on one screen is a signal to extract a private helper (its own `unit/<verb>_.c`).
- **Declare variables at the smallest scope that works**, as close as possible to first use.
- **Avoid unbounded or indirect recursion**; prefer an explicit loop or worklist when depth isn't statically bounded.
- **Assert invariants and preconditions** at function entry in non-trivial functions, in addition to handling recoverable errors as ordinary control flow.
- Ownership is explicit: document (in the header, or via the `_create` / `_destroy` naming convention) who allocates and who must free.
- Use function pointers (vtable-style structs) for runtime polymorphism, and opaque pointers (`struct foo *` with the definition hidden in the `.c`) for encapsulation — prefer these over exposing internals or reaching for non-C mechanisms.
- Use `const` for read-only parameters and data; avoid incidental copies.
- Use C23 `nullptr` for pointer null constants; `if (!ptr)` remains acceptable for guard clauses.
- Favor plain C23 features (`constexpr`, `[[nodiscard]]`, `typeof`, binary literals, digit separators) over macros or legacy workarounds when they express the same intent more directly.
- Use include guards or `#pragma once` consistently across the codebase — pick one and don't mix them.

## Header-only modules (when applicable)

For a module distributed as headers only:

- All implementation lives in `.h` / `.inl` files included by the public header.
- Mark per-translation-unit definitions `static inline` to avoid linker conflicts.
- Document which functions are safe to inline versus which should stay opaque (moved to a compiled unit instead).
- Keep the header self-contained — no external, non-standard-library dependencies.
- Show a minimal usage example in the header's top comment.

## What not to do

- Multiple unrelated functions in one `.c` file.
- A "god" `utils.c` / `helpers.c` accumulating reusable logic as `static` clusters instead of being split into their own `unit/` files.
- Listing a module's sources only in the root build file, bypassing a per-module build file.
- A lower-level module calling back into a higher-level/host module.
- Silently ignoring a function's return value when failure is possible.
- Unbounded dynamic allocation or recursion with no documented bound.
