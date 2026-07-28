---
name: instruction
description: the repo|codebase instructions.
required:
  - ".prompt/rules"
---


# Repository Guidelines `valve`

**NOTE**: This file is symlinked at multiple locations in the repository root:
- [`CLAUDE.md`, `GEMINI.MD`, `AGENTS.MD`] -> `.prompt/instruction.md`
- Always follow symlinks and edit the actual file, not copies.

## Policy

- **WARNING**: Never run long-running processes.
- **IMPORTANT**: Never use emoji. Use plain Unicode symbols — see rule: `unicode-symbols`.
- **WARNING**: Not a linter — never autoformat, reflow, or "tidy" lines.
- **IMPORTANT**: `.rules`, `.rule`, `.check`, `.follow`, `.done`, and `.handoff` are local-only coordination files. Keep them out of commits and ignore them at every repository depth.
- **IMPORTANT**: When exploring, adding, or editing files in any directory, check for a `.rules` file and read it first.
  - check also from the list of files per directory:
    - [
      ".rule" -> single rule, specific,
      ".check" -> todo check list,
      ".follow" -> live follow up of a /loop like audits,
      ".done" -> what is done from .check or plan specific,
      ".handoff" -> summary after a context clean-up,
      ".fix" -> requires attention based on severity,
    ]

## Project overview

valve is a CLI args parser library. Schema-driven: callers declare options (long, short, type, default), then parse argc/argv into a typed, queryable result set. Supports Linux and macOS with no third-party runtime dependencies.

## Rules index

Topic-scoped guidance lives under `.prompt/rules/`. Files with `paths:` frontmatter auto-activate when those paths are touched; files without `paths:` always load.

- `.prompt/rules/build-workflow.md` — rules for building and testing.
- `.prompt/rules/c-style.md` — C style preferences (scoped to `src/**/*.c`, `include/**/*.h`).
- `.prompt/rules/commit-guidelines.md` — commit format, signing, scope, and safety requirements.
- `.prompt/rules/dependencies.md` — dependencies and Meson build commands.
- `.prompt/rules/md-style.md` — writing and Markdown conventions (scoped to `**/*.md`).
- `.prompt/rules/plans.md` — plan conventions (scoped to `.prompt/plans/**/*.md`).
- `.prompt/rules/posix-cli-terminology.md` — standard command-line parsing terminology.
- `.prompt/rules/unicode-symbols.md` — plain Unicode alternatives to emoji.
- `.prompt/rules/version-bumping.md` — version and tag bumping rules.

## Workflows

- Plan files — keep active plans in `.prompt/plans/`, move completed plans to `.prompt/refs/plans/done/`, and see rule: `plans`.
- Version bumping — see rule: `version-bumping`.
- Commit conventions — see rule: `commit-guidelines`.

## License

Apache License 2.0. See `LICENSE`.
