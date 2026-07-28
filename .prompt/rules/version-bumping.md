---
name: version-bumping
description: Decision rules and tag format for version bumps. Consult whenever you cut a release, tag a commit, or update a version string in a project file.
---

# Version Bumping

You are the agent executing a bump. Follow these rules exactly.

## Decision: which component to bump

Evaluate in order. Take the FIRST match. If more than one applies, take the highest (MAJOR > MINOR > PATCH > BUILD).

1. Did the change break a public API, wire protocol, or on-disk format?
   -> Bump **MAJOR**. **Ask the user for explicit confirmation before tagging.**
2. Did the change add functionality without breaking existing API?
   -> Bump **MINOR**. No confirmation required.
3. Did the change fix a bug?
   -> **Ask the user** whether the fix is a **PATCH** (user-visible) or a **BUILD** (internal/trivial). Do not pick unilaterally.
4. Anything else (docs, refactor, whitespace, comment-only):
   -> Bump **BUILD**. No confirmation required.

## Tag format

- Shape: `vMAJOR.MINOR.PATCH-BUILD`
- All four components are decimal integers.
- Examples: `v0.1.0-000`, `v1.12.3-0042`, `v2.0.0-0000`.

## BUILD counter

- BUILD is zero-padded to the current width and incremented numerically.
  - `-009` -> `-010`, `-099` -> `-100`.
- When BUILD reaches the max of its width, bump PATCH and reset BUILD:
  - `v0.6.0-999` -> `v0.6.1-000`.

## BUILD width is a function of MINOR

- `MINOR` 0..9   -> BUILD width 3  (`-000` .. `-999`)
- `MINOR` 10..99 -> BUILD width 4  (`-0000` .. `-9999`)
- `MINOR` 100+   -> BUILD width grows by one digit per decade.
- Crossing a MINOR digit boundary (e.g. `0.9.x` -> `0.10.0`) starts the new tag at BUILD `-0000`. **Do NOT rewrite previously published tags** -- git tags are immutable.

## Reset semantics

- Bump **BUILD**: MAJOR, MINOR, PATCH unchanged.
- Bump **PATCH**: BUILD resets to padded zeros; MAJOR, MINOR unchanged.
- Bump **MINOR**: PATCH -> 0; BUILD -> padded zeros at the new width; MAJOR unchanged.
- Bump **MAJOR**: MINOR -> 0; PATCH -> 0; BUILD -> 3-digit padded zeros (MINOR is back to single digit).

## Unbounded components

- MAJOR has no digit cap.
- PATCH has no digit cap. PATCH does NOT drive BUILD width -- only MINOR does.

## Before you write the tag

1. Re-check the decision table. If MAJOR, or if ambiguity between PATCH and BUILD, you must have the user's explicit go-ahead in the current conversation.
2. Derive the new version from the **latest existing tag**, not memory. Use `git tag --list 'v*' --sort=-v:refname | head -1` or equivalent.
3. Apply reset semantics, then format with the correct BUILD width.
4. Write the tag, then mirror the same version string into every file listed under the project's `Where Version Appears` section.
