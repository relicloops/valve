# Valve reference

This guide describes Valve's parser behavior, schema relationships, terminal capabilities, error reporting, and ownership rules. Start with the root [README](../../README.md) for installation and the maintained [example programs](../example/README.md) for complete applications.

## Limitations

Valve does not implement operands. `VL_BEHAVIOR_ACCEPT_OPERANDS` is a reserved constant with no runtime effect, and there are no public operand accessors. `vl_parse()` reports an ordinary positional token as `VL_ERROR_UNEXPECTED_ARGUMENT`; when it encounters `--`, it stops parsing and currently discards every remaining argument.

Choose one assignment mode per executable. `VL_ASSIGN_INLINE` and `VL_ASSIGN_SEPARATE` cannot be enabled together, and Valve does not expand short-option clusters or joined option values.

## Color and terminal capabilities

Set `.color` on `vl_executable_t` to `VAL_COLOR_AUTO`, `VAL_COLOR_ALWAYS`, or `VAL_COLOR_NEVER`. A zero-initialized setting uses `VAL_COLOR_AUTO`, and `vl_create()` initializes color handling for the parser.

Environment variables take precedence over the schema setting:

1. A non-empty `NO_COLOR` disables color.
2. `FORCE_COLOR=0` disables color, `1` selects basic color, `2` selects 256 colors, and `3` or another non-empty value selects truecolor.
3. Without an environment override, Valve combines the schema mode with TTY, `TERM`, `COLORTERM`, and `TERM_PROGRAM` detection.

Use `vl_color_support_detect()`, `vl_color_supports()`, `vl_color_supports_256()`, `vl_color_supports_truecolor()`, `vl_color_supports_hyperlinks()`, and `vl_term_caps()` to query terminal capabilities. Valve's help output uses basic 8/16-color SGR sequences; the higher capability levels are available to callers. Valve does not expose a `vl_log_*` API.

## Errors and target lifecycle

Print structured parse errors and release parser and target state in this order:

```c
vl_errors_print(v, stderr);
vl_destroy(v);
vl_targets_clear(&settings);
```

`vl_errors_print()` writes errors as `error argv[<i>] [<key>]: <message>`. Passing a null stream selects `stderr`; use `vl_errors_foreach()` when an application needs a custom sink.

`vl_destroy()` releases parser-owned state but does not release values written into caller-owned targets. After handlers finish, call `vl_targets_clear()` to release strings and `VL_TARGET_VALUE` data reachable from the schema.

Release one `vl_value_t` with `vl_value_clear()`. Valve does not provide `vl_value_destroy()`.

## Parser domains and assignment modes

When a schema declares verbs, `argv[1]` must select a declared verb. Global options remain available in every verb domain, while verb-local options become available only after their owning verb is selected.

Without a verb table, Valve parses a single option domain beginning at `argv[1]`, so the first parsed token may be a caller-defined option.

`VL_ASSIGN_INLINE` accepts forms such as `--flag=value` and rejects `--flag value`. `VL_ASSIGN_SEPARATE` accepts `--flag value` and rejects the equals form. The maintained [`inline.c`](../example/inline.c) and [`separate.c`](../example/separate.c) programs demonstrate the same schema under both modes.

## Executable metadata

Populate `.program_name`, `.program_version`, `.description`, and `.usage` directly on `vl_executable_t`. These are the preferred and functional metadata fields in experimental `1.x`; `vl_create()` copies their strings into parser-owned state.

The nested `.metadata` aggregate remains in the public structure for an unfinished migration but is not read by `vl_create()`. Do not populate it as a substitute for the direct fields.

## Values and targets

- Repeatable options become arrays only when declared with `VL_OPTION_REPEAT_ARRAY`; duplicate non-repeatable options fail.
- Integer minimum and maximum bounds are enforced during parsing.
- `VL_OPTION_VALUE_BOOL` accepts a bare flag as true in every assignment mode and accepts explicit boolean literals in inline mode.
- `VL_OPTION_VALUE_AUTO` converts `true` and `false` to booleans, integers to `int`, decimals to `double`, and other unquoted scalars to strings.
- `VL_OPTION_VALUE_KV` builds structured key/value trees. Nested values use `{!...}`, and quoted scalar leaves remain strings.
- `VL_OPTION_VALUE_TIME` converts durations to whole seconds before applying integer bounds.
- Dotted option names are parsed and validated as schema keys.

Use `.data`, `.offset`, and `.target` to write parsed values into caller-owned configuration fields. Initialize string and `VL_TARGET_VALUE` fields to zero before parsing.

String targets receive allocated copies. `VL_TARGET_VALUE` targets receive deep copies for compound values such as arrays and key/value trees; release both through `vl_targets_clear()` after the application finishes using them.

## Required options

Set `.required = true` when an option must appear in `argv`. If it is absent, `vl_parse()` reports `VL_ERROR_MISSING_REQUIRED`.

Valve checks required global options and options in the active verb chain. Reserved `--help`, `--version`, and `--valve` requests skip required-option checks.

Generated help marks required options and derives each signature and placeholder from `.type`, `.value`, `.target`, and the executable assignment mode. Optional `.usage` text appears beneath the generated signature but does not replace it.

## Option requirements

Use `.requires` to declare options that must accompany a source option:

```c
static const vl_option_t *const reload_requires[] = {
    &id,
    NULL,
};

const vl_option_t reload = {
    .name = "enable-reload",
    .requires = reload_requires,
};
```

A zero `.require_count` treats the table as null-terminated; otherwise, Valve uses the explicit count.

Requirements are directed and conditional. `--id` may appear alone, but `--enable-reload` fails with `VL_ERROR_UNSATISFIED_REQUIREMENT` unless `--id` also appears. Presence controls the relationship, so `--enable-reload=false` still requires `--id`.

Generated help annotates both sides as `[requires: --id]` and `[required by: --enable-reload]`. The `.required` field remains unconditional in the active scope, while `.requires` applies only when its source option appears.

## Conflicting options

Use `.conflicts` to declare options that cannot appear together:

```c
static const vl_option_t *const limit_conflicts[] = {
    &no_limit,
    &automatic,
    NULL,
};

const vl_option_t limit = {
    .name = "limit",
    .conflicts = limit_conflicts,
};
```

A zero `.conflict_count` treats the table as null-terminated; otherwise, Valve uses the explicit count.

One declaration is bidirectional. Valve reports `VL_ERROR_CONFLICTING_OPTION` when both options appear and annotates both generated help entries without requiring a reverse declaration.

Conflicts depend on presence rather than truthiness, so an explicit `--no-limit=false` still conflicts with `--limit`.

## Verbs and sub-verbs

A verb may declare one nested verb table through `.verbs` and `.verb_count`. When that table exists, a sub-verb token is mandatory immediately after the parent verb.

Option lookup checks the active sub-verb, its parent verb, and then globals. `vl_verb_get()` returns the parent name, while `vl_subverb_get()` returns the selected leaf. Missing and unknown sub-verbs report `VL_ERROR_MISSING_COMMAND` and `VL_ERROR_UNKNOWN_COMMAND`, respectively.

## Stability and versioning

Valve uses `MAJOR.MINOR.PATCH-BUILD` versions and matching `vMAJOR.MINOR.PATCH-BUILD` tags. During the experimental `1.x` period, a minor release may change source or ABI compatibility; pin an exact tag when consuming Valve through a Wrap.

The project will lift the experimental status explicitly in its release notes and status badge. After that transition, changes follow the compatibility meaning of the first three components: breaking public API or ABI changes increment `MAJOR`, backward-compatible functionality increments `MINOR`, and compatible fixes increment `PATCH`. `BUILD` remains the published-build counter and does not weaken those compatibility promises.

## Schema and ownership

- `vl_create()` copies option and verb dictionaries.
- When `option_count` is zero, the option pointer table must end with `NULL`.
- When `verb_count` is zero, the verb pointer table must end with `NULL`.
- `vl_parse()` never owns the `argv` passed to `main()`.
- Use `vl_argv_destroy()` only for an argument array the caller explicitly allocated and wants Valve to release.

See the public [`valve.h`](../../include/valve/valve.h) header for declarations and the [example programs](../example/README.md) for complete parse, error, handler, and teardown flows.
