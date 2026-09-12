---
name: command-option
description: A schema-declared option that receives the command after `--` — the tail stops being discarded
severity: high
status: released 2026-09-02; Valve v1.1.0-001 and public docs source complete. Key dependency raised; Key exec runtime remains in its own plan
---

# Command option

Valve stops parsing at `--` and drops everything after it. The terminology rule already records that as incorrect partial behaviour, and the road-map prices full operands as the most expensive missing piece: storage, accessors, lifecycle, disambiguation of unprefixed tokens before `--`, and help. This plan does not do operands. It closes the one case that needs none of that machinery: a schema that wants the tail after `--` as a command to run.

The case is concrete. `key exec -- generator cron list` has to hand `generator cron list` to `execvp` untouched, with `--help` or any option inside the tail belonging to generator and not to Valve. The tail is opaque by definition, so there is nothing to disambiguate, and the schema can say who receives it.

## Decision

One new option value and one new target. An option with `.value = VL_OPTION_VALUE_COMMAND` is never matched by name from a token. It is the schema's declaration that the verb chain accepts a `--` tail, and the tail lands on its target.

```c
/* A command captured after `--`. argv is NULL-terminated and points into the
 * argv given to vl_parse, so it goes to execvp unchanged. Nothing is copied
 * and nothing is freed; it lives as long as that argv does. A caller using a
 * command option must provide the main-style argv[argc] == NULL sentinel. */
typedef struct vl_command {
  char *const *argv;
  int argc;
} vl_command_t;

VL_OPTION_VALUE_COMMAND   /* appended to vl_option_value_t */
VL_TARGET_COMMAND         /* appended to vl_target_t */
VL_VALUE_COMMAND          /* appended to vl_value_kind_t; read as .as.command */
```

Two constraints fix that shape. Valve is consumed as a shared library, so the size of every public struct holds: a two-field command struct is sixteen bytes, the same as the array and kv members already in the union, so `vl_value_t` does not grow and the ABI is intact. And the schema already requires `.name`, which is the result key, the help target and the `.required` message, so it stays: `vl_get(v, "command")` returns a `VL_VALUE_COMMAND`, and a consumer reads `argv[0]` as the executable and the rest as its arguments.

## Parse rules

- On `--`, resolve the command option through the active scope, sub-verb first, then verb, then globals, the order option lookup already uses. With none declared the tail is discarded exactly as today: existing schemas keep their behaviour, and rejecting an unowned tail stays with the operands plan.
- With one declared and the tail empty, or its first token an empty string, `VL_ERROR_MISSING_VALUE` with the message `missing command after --`. A command with no executable means nothing, unlike `rm --`.
- The tail is never interpreted. Reserved tokens, options and enable/disable forms are not examined past `--`. The loop already leaves at that token, so this is a test rather than code.
- The result's `argv_index` is the index of `--`, consistent with every other option recording the token that produced it. Repeat is `VL_OPTION_REPEAT_ERROR` by construction, because a second `--` is inside the tail.
- `--command=x` against the option's own name is refused as `VL_ERROR_DISABLED_FORM` with `command is given after --`, so a user who guesses the name is told the form rather than `unknown option`.
- A schema with a command option requires a main-style `argv` vector whose `argv[argc]` entry is `NULL`. Ordinary schemas retain the existing `argc`-bounded contract; the sentinel requirement exists only where Valve returns an `execvp`-ready borrowed tail.

## Schema rules

Validated in `vl_create`, so a wrong declaration fails at startup like every other schema error.

- `.type` is zero, no `short_name`, no `toggle_ref`. The command has no syntactic form.
- `.target` is `VL_TARGET_NONE`, `VL_TARGET_COMMAND` or `VL_TARGET_VALUE`.
- `.repeat` is `VL_OPTION_REPEAT_ERROR`.
- At most one command option per chain. Two sub-verbs may each own one; a sub-verb and its verb may not both, and neither may a verb and the globals.
- `.required`, `.conflicts` and `.requires` work unchanged because they act on result presence. A consumer that marks the option required gets `missing required option: command` for free.

## Targets and lifecycle

`VL_TARGET_COMMAND` copies the struct to `.data + .offset`; `vl_targets_clear` zeroes it. `vl_value_clear` frees nothing for a command, and the result clone in `vl_result_set_` is a shallow copy. Borrowing rather than copying is the point: Spool destroys the parser before dispatch, and the target on the caller's config struct must still be valid then. It is, because it points into `main`'s argv.

## Help

The usage printer emits `-- <command> [args…]` for the option, the value label and relationship label describe that form, and the option is listed last in its scope so the help reads in argument order. Targeted option help uses the command form rather than inventing `--command`.

## What does not change

`VL_BEHAVIOR_ACCEPT_OPERANDS` stays reserved and unimplemented. When operands land, unprefixed tokens before `--` become operands, and the tail after `--` goes to the command option when one is in scope and to operands otherwise. That precedence is the one sentence the operands plan inherits from this one.

Whether `--` with no command option in scope should keep discarding the tail is left as it is. Keeping it is zero risk. Rejecting it is more correct and is a behaviour change for every consumer that ever passed `--` by accident, so it belongs to the operands plan, decided on its own.

## Source

Units follow the one-function-per-file rule. Each new unit is added to the Meson source list, the canonical Ceedling list under `test/support/`, and every test-file Ceedling preamble that links the full library. No tracked generator currently synchronizes those preambles.

- `include/valve/valve.h`: the three enum values, `vl_command_t`, the union member, doc comments stating the borrowed lifetime.
- `src/valve/tu/option/command_valid_.c`: the schema shape rules, called from `options_have_invalid_`.
- `src/valve/tu/option/options_command_count_.c`: counts command options in a table; `schema/validate_.c` refuses more than one across globals, verb and sub-verb.
- `src/valve/tu/option/find_command_.c`: `vl_option_find_command_` over the active scope.
- `src/valve/tu/parse.c`: the `break` at `--` calls the separately implemented `vl_parse_command_`; `parse_long_` gains the disabled-form refusal for a command option matched by name.
- `result/set_.c`, `value/clear.c`, `targets_clear.c`, `valve.c` target validity, `option/value_valid_.c`, `option/value_label_.c`, `option/label_.c`, `option/usage_print_.c`, and `help_print_.c`: command cases for copying, clearing, relationship diagnostics, usage, ordering, and targeted help.
- `test/test_command.c`: tail captured with pointer identity into a main-style NULL-terminated argv; empty tail refused; `--help` in the tail not fired; disabled form on the name; each schema rule refused; one per chain enforced; sub-verb scope resolved; `--` without a declared option still discards; required, conflicts and requires checks; direct and `VL_TARGET_VALUE` targets survive `vl_destroy`; targets clear; help ordering and targeted help render the command form; `vl_get` kind. Branch-coverage annotations where a branch is unreachable, as the existing units carry them.
- README, `docs/reference/README.md`, the status table in the terminology rule, and the road-map: the `--` row moves from incorrect partial behaviour to a schema-owned command tail, operands still missing.

## Version and release

Valve is public, on GitHub and at valve.relicloops.org, so this ships as a release and not as a commit.

The version is `1.1.0-001`, set by the maintainer 2026-09-02. MINOR because the change adds public API without breaking any, and BUILD `-001` because the counter is monotonic across the element's life and never resets, the rule the fleet already follows in key. The local `version-bumping` rule still describes a reset on MINOR and is aligned with the fleet rule in the same change, so the next bump does not re-derive from two texts.

Where the version appears, all eight active occurrences are mirrored together before the changelog: `meson.build`; `VALVE_VERSION` and `PROGRAM_VERSION` in `src/valve/private.h`; `project.yml`; `test/ceedling-gcov.yml`; and the README badge, wrap revision and current-tag sentence.

Sequence: source and tests green, `git-cliff --output CHANGELOG.md`, version mirrored, signed commit, annotated tag `v1.1.0-001`, GitHub Release, docs site regenerated. The README's consumer section then names `v1.1.0-001` as the tag to pin.

## Fleet

The library installs as `libvalve.so.0` and the soname stays `0`: enum values are appended and no public struct changes size, so a consumer built against 1.0.1 keeps running on 1.1.0 without a rebuild. On vm-relicloops both library versions may sit on the box during the migration; that is a prefix decision recorded in `core.os-arm-v1`, since Valve is not a sysext element and reaches the box through pkg-config and `build-native`.

Key migrates first. Its `exec` verb declares the command option, `.required`, filling a `vl_command_t` on its argv aggregate, and its meson dependency becomes `dependency('valve', version: '>=1.1.0', static: false)`. The other fleet elements relink at their own next release; nothing forces them, and nothing in their schemas changes.

## Todo check list

- [x] Public header: enums, `vl_command_t`, union member, lifetime doc.
- [x] `command_valid_`, `options_command_count_`, schema validation for one per chain.
- [x] `find_command_` and `parse_command_`; disabled form on the name.
- [x] Result set, value clear, targets clear, target validity, value validity, relationship label, usage, help ordering and targeted help.
- [x] Main-style NULL sentinel contract documented for command schemas without changing ordinary argc-bounded schemas.
- [x] All full-library Ceedling preambles, the canonical support list and Meson source list synchronized for new units.
- [x] `test/test_command.c` with the cases above, including conflicts/requires and post-`vl_destroy` target lifetime; coverage at the repo's bar.
- [x] README, reference docs, terminology status table, road-map.
- [x] Align the local `version-bumping` rule with the fleet's monotonic BUILD.
- [x] Changelog, all eight active version occurrences mirrored, signed commit, tag `v1.1.0-001`, GitHub Release, docs site regenerated.
- [x] Key dependency: `dependency('valve', version: '>=1.1.0', static: false)`.
- [ ] Key `exec` command option and runtime route — tracked by Key's separate `06-key-exec` plan; do not register a nonfunctional verb here.

## Not in scope

Operands before `--`. Rejecting an unowned `--` tail. Short stacking, joined forms, mixed assignment modes. A copying variant of `vl_command_t`; if a consumer ever needs the tail to outlive argv, it copies.
