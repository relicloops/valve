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

- BUILD is the element's lifetime release counter. It increments numerically for every published version and never resets when MAJOR, MINOR, or PATCH changes.
- BUILD is zero-padded to at least three digits.
  - `-009` -> `-010`, `-099` -> `-100`.
- The field grows naturally beyond its current width: `-999` -> `-1000`.
- The first published version of a new element may use `-000`; every later release uses the next lifetime BUILD value.

## Component semantics

- Bump **BUILD**: MAJOR, MINOR, PATCH unchanged; increment BUILD.
- Bump **PATCH**: increment PATCH; increment BUILD; MAJOR and MINOR unchanged.
- Bump **MINOR**: increment MINOR; set PATCH to 0; increment BUILD; MAJOR unchanged.
- Bump **MAJOR**: increment MAJOR; set MINOR and PATCH to 0; increment BUILD.

## Unbounded components

- MAJOR has no digit cap.
- PATCH has no digit cap.
- BUILD has no digit cap.

## Before you write the tag

1. Re-check the decision table. If MAJOR, or if ambiguity between PATCH and BUILD, you must have the user's explicit go-ahead in the current conversation.
2. Derive the new version from the **latest existing tag**, not memory. Use `git tag --list 'v*' --sort=-v:refname | head -1` or equivalent.
3. Apply component semantics and increment the lifetime BUILD, then format it with at least three digits.
4. Write the tag, then mirror the same version string into every file listed under the project's `Where Version Appears` section.
