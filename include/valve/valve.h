/**
 * @file valve.h
 * Public API of Valve, a schema-driven command-line argument parser.
 *
 * The caller declares an executable (vl_executable_t) made of global options,
 * verbs with their own options and optional sub-verbs, and optional
 * executable-level actions. vl_create() validates and copies that schema
 * into an opaque valve_t; vl_parse() then turns a main-style argv into a
 * typed, queryable result set (vl_get(), vl_result_at()) plus a list of
 * parse errors (vl_error_at(), vl_errors_print()).
 *
 * Ownership rules:
 *  - Schema strings are copied by vl_create(); the caller may release them
 *    afterwards. Option tables and verb tables are copied as well.
 *  - Option `.data` target pointers stay caller-owned. Heap memory Valve
 *    writes into them is released by vl_targets_clear(), never by
 *    vl_destroy().
 *  - Every pointer returned from a `const valve_t *` query belongs to the
 *    parser and is invalidated by the next vl_parse() or by vl_destroy().
 *
 * Reserved tokens `--help` / `-h` / `-?`, `--version` / `-v` and `--valve`
 * are intercepted at parse time unless VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED
 * lets a user option or verb claim the name.
 */

#ifndef VALVE_H
#define VALVE_H

#include "valve/color.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct valve valve_t; /**< Opaque parser handle (see vl_create()). */
typedef struct vl_metadata vl_metadata_t;
typedef struct vl_option vl_option_t;
typedef struct vl_executable_action vl_executable_action_t;

typedef struct vl_verb vl_verb_t;

/** Bit flags altering vl_create() validation and vl_parse() behavior.
 *  Combine with `|` in vl_executable_t::behavior. */
typedef enum vl_behavior {
  /** When set, vl_create accepts user options/verbs using reserved names
   *  (--help, --version, --valve and their aliases). At parse time, a
   *  user-defined option/verb wins over the reserved interception. */
  VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED = 1u << 0,
  /** Reserved for future operand support. This flag is not implemented and
   *  currently has no effect. vl_parse rejects ordinary positional tokens;
   *  `--` stops parsing and the remaining argv tail is ignored. */
  VL_BEHAVIOR_ACCEPT_OPERANDS = 1u << 1,
  /** When set, the executable's `actions` table joins the reserved set:
   *  each action is recognised at argv[1] like --help / --version / --valve,
   *  fires without a verb, and is listed on the reserved help line.
   *  Actions must be opted into explicitly: vl_create returns NULL when
   *  `action_count` is non-zero and this flag is clear. With the flag set
   *  and no actions declared, it has no effect. */
  VL_BEHAVIOR_ACCEPT_NO_VERB = 1u << 2,
} vl_behavior_t;

/** How an option-argument attaches to its option. The choice is
 *  executable-global: one parser accepts exactly one style. Bare boolean
 *  flags (`--flag`, `-f`) are accepted in either mode. */
typedef enum vl_assign {
  VL_ASSIGN_INLINE = 0, /**< --flag=value, -f=value */
  VL_ASSIGN_SEPARATE,   /**< --flag value, -f value */
} vl_assign_t;

/** Reserved migration aggregate for program metadata. Not read by
 *  vl_create() in experimental 1.x; see vl_executable_t::metadata. */
struct vl_metadata {
  const char *program_name;    /**< reserved */
  const char *program_version; /**< reserved */
  const char *description;     /**< reserved */
  const char *usage;           /**< reserved */
};

/** Executable schema handed to vl_create(). Zero-initialise and set the
 *  fields you need; every pointer may be NULL and every count may be 0.
 *  Tables are arrays of pointers; a count of 0 with a non-NULL table means
 *  the table is NULL-terminated. */
typedef struct vl_executable {

  vl_assign_t assign;     /**< option-argument style (default INLINE) */
  vl_behavior_t behavior; /**< vl_behavior_t flags (default none) */

  /** Reserved migration aggregate. vl_create does not read this field in
   *  experimental 1.x; populate the direct metadata fields below. */
  const vl_metadata_t metadata;
  /** Preferred program metadata fields for experimental 1.x. Strings are
   *  copied by vl_create and may be released by the caller afterward. */
  const char *program_name;    /**< shown in help/version banners */
  const char *program_version; /**< shown by --version */
  const char *description;     /**< one-line summary shown in help */
  const char *usage;           /**< optional usage line shown in help */

  /** Brand glyph shown by the default help/version banner (e.g. "❖").
   *  Defaults to "❅" when NULL. */
  const char *logo;

  /**
   * Preferred ANSI color mode for help/version banners (and any later
   * vl_color_for use). Applied once in vl_create via vl_color_init.
   * Precedence inside vl_color_init / term detect (align with ansi lib):
   *   1. NO_COLOR non-empty              → force NONE
   *   2. FORCE_COLOR leveled (0/1/2/3)   → force NONE/BASIC/256/TRUECOLOR
   *   3. this field (AUTO / ALWAYS / NEVER); ALWAYS ≈ at least BASIC
   *      AUTO runs capability detect (TTY + TERM/COLORTERM/…)
   * Default when unset (0): VAL_COLOR_AUTO.
   */
  vl_color_mode_t color;

  /** Verb table. When non-empty, argv[1] must name a verb (or a reserved
   *  token / action); otherwise vl_parse() reports VL_ERROR_MISSING_COMMAND
   *  or VL_ERROR_UNKNOWN_COMMAND. Verb names must be unique. */
  const vl_verb_t *const *verbs;
  size_t verb_count; /**< entries in `verbs`; 0 → NULL-terminated */

  /** Global options, visible in every verb / sub-verb scope. Long and short
   *  names must be unique across the table and must not collide with any
   *  verb-scoped option. */
  const vl_option_t *const *options;
  size_t option_count; /**< entries in `options`; 0 → NULL-terminated */

  /** Executable-level actions (see vl_executable_action_t). Recognised at
   *  argv[1] only, in the same position as --help / --version / --valve. */
  const vl_executable_action_t *const *actions;
  size_t action_count; /**< entries in `actions`; requires ACCEPT_NO_VERB */

  /** Replaces the default help printer when a help token fires. The
   *  requested target, if any, is available via vl_help_target(). */
  void (*on_help)(const valve_t *v);
  /** Replaces the default version printer when a version token fires. */
  void (*on_version)(const valve_t *v);
  /** Replaces the default `--valve` printer (library banner and version). */
  void (*on_valve)(const valve_t *v);

} vl_executable_t;

/** A verb (`program verb ...`): a named scope with its own options and,
 *  optionally, a nested sub-verb table. Strings and tables are copied by
 *  vl_create(). */
typedef struct vl_verb {
  const char *name;        /**< required; matched exactly against argv[1] */
  const char *description; /**< one-line summary shown in help */
  const char *usage;       /**< optional usage guidance shown in verb help */
  /** Options valid only under this verb (and its sub-verbs). Names must not
   *  collide with global options. */
  const vl_option_t *const *options;
  size_t option_count; /**< entries in `options`; 0 → NULL-terminated */
  /** Optional nested verb table (sub-verbs): `program verb subverb ...`.
   *  When present, a sub-verb token is mandatory after the verb. Option
   *  lookup covers sub-verb options, then the owning verb's, then globals.
   *  The active names are read with vl_verb_get() / vl_subverb_get(). */
  const vl_verb_t *const *verbs;
  size_t verb_count; /**< entries in `verbs`; 0 → NULL-terminated */
} vl_verb_t;

/** Option type -- selects accepted syntactic form.
 * LONG and SHORT may be combined (VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT).
 * TOGGLE is mutually exclusive with the other two; combining them causes
 * vl_create() to return NULL.
 *
 * LONG  requires .name.
 * SHORT requires .short_name.
 * TOGGLE requires .toggle_ref (base name matched after --enable-/--disable-
 *        prefix stripping).
 */
typedef enum vl_opt_type {
  VL_OPT_TYPE_LONG = 1u << 0,   /**< --flag */
  VL_OPT_TYPE_SHORT = 1u << 1,  /**< -f */
  VL_OPT_TYPE_TOGGLE = 1u << 2, /**< --enable-flag / --disable-flag */
} vl_opt_type_t;

/** How an option's option-argument is parsed and which vl_value_kind_t it
 *  produces. A parse failure is reported as VL_ERROR_INVALID_VALUE. */
typedef enum vl_option_value {
  /** Infer the kind from the literal: `true` / `false` → BOOL, a strict
   *  decimal integer → INT, a decimal with fraction or exponent → DOUBLE,
   *  anything else → STRING. Hex, `inf` and `nan` stay text. */
  VL_OPTION_VALUE_AUTO = 0,
  VL_OPTION_VALUE_STRING, /**< stored verbatim as VL_VALUE_STRING */
  /** Decimal integer stored as VL_VALUE_INT and bounded by `.int_min` /
   *  `.int_max` when the matching `has_*` flag is set. */
  VL_OPTION_VALUE_INT,
  VL_OPTION_VALUE_DOUBLE, /**< decimal number stored as VL_VALUE_DOUBLE */
  /** Integer or double: a literal with a fraction / exponent becomes
   *  VL_VALUE_DOUBLE, otherwise VL_VALUE_INT (int bounds apply). */
  VL_OPTION_VALUE_NUMBER,
  /** Key/value map stored as VL_VALUE_KV. Grammar: `key:value|key:value`,
   *  with `{ ... }` for nested maps and `"..."` quoting (`\"` and `\\`
   *  escapes) around keys or values. Scalar values are typed as for AUTO.
   *  Long form only; the short form reports VL_ERROR_DISABLED_FORM. */
  VL_OPTION_VALUE_KV,
  /** Enable/disable toggle stored as VL_VALUE_BOOL: `--enable-<ref>` sets
   *  true, `--disable-<ref>` sets false. Required for VL_OPT_TYPE_TOGGLE. */
  VL_OPTION_VALUE_TOGGLE,
  /** Comma-separated list stored as VL_VALUE_ARRAY. Each element is typed
   *  as for AUTO; a `"..."` element is always a string. Empty lists and
   *  trailing commas are rejected. */
  VL_OPTION_VALUE_ARRAY,
  /** Boolean flag: bare `--flag` (any assign mode) means true;
   *  `--flag=true` / `--flag=false` are accepted in inline mode. */
  VL_OPTION_VALUE_BOOL,
  /** Grouped option: the NAME is a dotted `group.leaf` path (e.g.
   *  `proxy.lane`). The option still matches by exact name; the dotted form
   *  drives grouped help/usage. The stored scalar kind is taken from
   *  `.target` (VL_TARGET_STRING/INT/...), not from this enum. */
  VL_OPTION_VALUE_DOT_NOTATION,
  /** Duration literal, stored as whole seconds in a VL_VALUE_INT. Accepts a
   *  non-negative integer with an optional `s`/`m`/`h`/`d` suffix, and
   *  compound forms such as `1h30m`. A bare integer means seconds, so an
   *  option migrating from VL_OPTION_VALUE_INT keeps accepting its old
   *  inputs. Because the parsed value is seconds, `.has_int_min` /
   *  `.has_int_max` still bound it, and a VL_TARGET_INT or VL_TARGET_INT64
   *  target receives the converted count. */
  VL_OPTION_VALUE_TIME,
  /** Opaque command tail captured after `--`. This value has no long, short,
   *  or toggle form; `.name` remains its result key and help target. */
  VL_OPTION_VALUE_COMMAND,
} vl_option_value_t;

/** What happens when the same option appears more than once in argv. */
typedef enum vl_option_repeat {
  /** Second occurrence reports VL_ERROR_DUPLICATE_OPTION. */
  VL_OPTION_REPEAT_ERROR = 0,
  /** Occurrences accumulate into one VL_VALUE_ARRAY result (the first
   *  occurrence is promoted to an array on the second). */
  VL_OPTION_REPEAT_ARRAY,
} vl_option_repeat_t;

/** Type of the caller-owned storage an option writes into. The address is
 *  `(char *)data + offset`; see vl_option_t::data. A kind mismatch between
 *  the parsed value and the target reports VL_ERROR_INVALID_VALUE. */
typedef enum vl_target {
  VL_TARGET_NONE = 0, /**< no target; read the value with vl_get() */
  /** `char *`: receives a strdup() copy (previous copy is freed). Release
   *  with vl_targets_clear(). Requires a VL_VALUE_STRING result. */
  VL_TARGET_STRING,
  VL_TARGET_INT,    /**< `int`; VL_VALUE_INT that fits in int */
  VL_TARGET_INT64,  /**< `int64_t`; VL_VALUE_INT */
  VL_TARGET_DOUBLE, /**< `double`; VL_VALUE_DOUBLE */
  VL_TARGET_BOOL,   /**< `bool`; VL_VALUE_BOOL */
  /** `vl_value_t`: receives a deep copy of any value kind (KV, ARRAY, ...).
   *  Release with vl_targets_clear() or vl_value_clear(). */
  VL_TARGET_VALUE,
  /** `vl_value_t`: same storage as VL_TARGET_VALUE, named for toggle
   *  options. */
  VL_TARGET_TOGGLE,
  /** `vl_command_t`: receives the borrowed command view (no copy). Only
   *  valid with VL_OPTION_VALUE_COMMAND. */
  VL_TARGET_COMMAND,
} vl_target_t;

/** One option declaration. Strings are copied by vl_create(); `.data`,
 *  `.conflicts` and `.requires` reference caller memory that must outlive
 *  the parser. */
typedef struct vl_option {
  /** Required. Long name without dashes (`verbose`, `proxy.lane`); also the
   *  result key for vl_get() and the help target. Must not start with `-`
   *  or contain `=`. */
  const char *name;
  /** Single-character short name (`v` for `-v`); required for
   *  VL_OPT_TYPE_SHORT. Must not be `-` or `=`. */
  char short_name;
  /** Base name for VL_OPT_TYPE_TOGGLE (`cache` matches `--enable-cache` and
   *  `--disable-cache`). Required for TOGGLE, ignored otherwise. */
  const char *toggle_ref;
  const char *description; /**< one-line summary shown in help */
  /** Optional usage guidance printed below Valve's inferred option syntax. */
  const char *usage;
  vl_opt_type_t type;        /**< accepted syntactic forms */
  vl_option_value_t value;   /**< option-argument parsing rule */
  vl_option_repeat_t repeat; /**< policy for repeated occurrences */
  /** When true, vl_parse() fails with VL_ERROR_MISSING_REQUIRED if the
   *  option was never provided. Checked for globals and the active verb
   *  chain; skipped when a reserved token (--help/--version/--valve) or an
   *  executable action fires. */
  bool required;
  /** Options that cannot appear with this option. One declaration establishes
   *  the relationship in both directions for parsing and default help. A
   *  count of 0 means the table is NULL-terminated. */
  const vl_option_t *const *conflicts;
  size_t conflict_count; /**< entries in `conflicts` */
  /** Options that must also appear when this option appears. The relationship
   *  is directed. A count of 0 means the table is NULL-terminated. */
  const vl_option_t *const *
    requires;
  size_t require_count; /**< entries in `requires` */
  /** Base address of caller-owned target storage (e.g. a config struct).
   *  NULL disables target population; the value is still stored in the
   *  result set. Never freed by Valve. */
  void *data;
  size_t offset;      /**< byte offset of the field inside `data` */
  vl_target_t target; /**< type of the field at `data + offset` */
  bool has_int_min;   /**< enforce `int_min` on INT / NUMBER / TIME values */
  bool has_int_max;   /**< enforce `int_max` on INT / NUMBER / TIME values */
  int64_t int_min;    /**< inclusive lower bound when `has_int_min` */
  int64_t int_max;    /**< inclusive upper bound when `has_int_max` */
} vl_option_t;

/** Executable-level action: an option that acts on the executable itself
 *  rather than on a verb's domain, in the same category as --help, --version
 *  and --valve. Enabled by VL_BEHAVIOR_ACCEPT_NO_VERB. It is recognised at
 *  argv[1] only, never enters the option lookup chain (`program verb --name`
 *  is an unknown option), and once it fires no verb is required and nothing
 *  else may follow it on the command line.
 *
 *  `option` is parsed with the ordinary option rules: `.type` selects
 *  --name / -c / --enable-ref forms, `.value` decides whether a bare token is
 *  accepted (BOOL / TOGGLE) or an option-argument is required in the
 *  executable's assign mode, `.target` / `.data` / `.offset` and the int
 *  bounds apply as usual, and help infers the signature from them. The
 *  parsed value is stored under `.name` and readable with vl_get(). Fields
 *  without meaning for an action make vl_create() return NULL when set:
 *  `.required`, `.repeat`, `.conflicts`, `.requires`, and
 *  VL_OPTION_VALUE_COMMAND. Names may not collide with the built-in reserved
 *  names, another action, or a global option, regardless of
 *  VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED. Strings are copied by vl_create;
 *  `.data` stays caller-owned, as for options. */
struct vl_executable_action {
  vl_option_t option; /**< the option that triggers the action */
  /** Required. Called after the value is stored and targets are populated;
   *  vl_reserved_fired() and vl_action_fired() are already set inside. */
  void (*run)(const valve_t *v);
};

typedef struct vl_kv_pair vl_kv_pair_t;
typedef struct vl_value vl_value_t;

/** Ordered key/value pairs of a VL_VALUE_KV value, in input order. */
typedef struct vl_kv_list {
  const vl_kv_pair_t *pairs; /**< owned by the enclosing vl_value_t */
  size_t count;              /**< number of pairs */
} vl_kv_list_t;

/** Elements of a VL_VALUE_ARRAY value, in input order. */
typedef struct vl_array {
  const vl_value_t *items; /**< owned by the enclosing vl_value_t */
  size_t count;            /**< number of elements */
} vl_array_t;

/** Borrowed command tail captured after `--`.
 *
 * `argv` points into the vector passed to vl_parse() and is never copied or
 * freed by Valve. A schema declaring VL_OPTION_VALUE_COMMAND requires a
 * main-style input vector with `argv[argc] == NULL`, which makes this slice
 * directly suitable for execvp(). The strings and vector must outlive every
 * use of this value. */
typedef struct vl_command {
  char *const *argv; /**< first token after `--`; `argv[argc]` is NULL */
  int argc;          /**< number of tokens after `--` (always >= 1) */
} vl_command_t;

/** Runtime kind of a parsed vl_value_t; selects the active `as` member. */
typedef enum vl_value_kind {
  VL_VALUE_STRING = 0, /**< text; read `raw` */
  VL_VALUE_INT,        /**< `as.integer` */
  VL_VALUE_DOUBLE,     /**< `as.number` */
  VL_VALUE_BOOL,       /**< `as.boolean` */
  VL_VALUE_KV,         /**< `as.kv` */
  VL_VALUE_ARRAY,      /**< `as.array` */
  VL_VALUE_COMMAND,    /**< `as.command` (borrowed, see vl_command_t) */
} vl_value_kind_t;

/** A parsed value. Values returned by vl_get() / vl_result_at() are
 *  parser-owned; values copied into a VL_TARGET_VALUE / VL_TARGET_TOGGLE
 *  target are caller-owned and released with vl_value_clear(). */
struct vl_value {
  vl_value_kind_t kind; /**< which `as` member is valid */
  /** Original option-argument text (owned copy). NULL for a COMMAND value
   *  and for an array built from repeated options. */
  const char *raw;

  union {
    int64_t integer;      /**< VL_VALUE_INT */
    double number;        /**< VL_VALUE_DOUBLE */
    bool boolean;         /**< VL_VALUE_BOOL */
    vl_kv_list_t kv;      /**< VL_VALUE_KV */
    vl_array_t array;     /**< VL_VALUE_ARRAY */
    vl_command_t command; /**< VL_VALUE_COMMAND */
  } as;                   /**< payload selected by `kind` */
};

/** One entry of a VL_VALUE_KV map. */
struct vl_kv_pair {
  const char *key;  /**< owned copy of the key text */
  vl_value_t value; /**< scalar or nested VL_VALUE_KV */
};

/** One stored option result, in first-seen order. */
typedef struct vl_result {
  const char *key;  /**< option `.name` (parser-owned) */
  vl_value_t value; /**< parsed value (parser-owned) */
  /** argv index of the option token that produced the value (the `--` for
   *  a command; the latest occurrence for a repeated option). */
  int argv_index;
} vl_result_t;

/** Reason a vl_error_t was recorded. */
typedef enum vl_error_code {
  VL_ERROR_UNKNOWN_OPTION = 1, /**< no option matches the token */
  /** The option exists but not in that form: wrong long/short/toggle type,
   *  inline vs separate mismatch with the executable's assign mode, KV via
   *  short form, or a command option given outside `--`. */
  VL_ERROR_DISABLED_FORM,
  VL_ERROR_MISSING_VALUE, /**< option-argument or `--` tail absent */
  /** Option-argument failed parsing, bounds, target-type checks, or an
   *  unknown `--help=<target>`. */
  VL_ERROR_INVALID_VALUE,
  /** Positional token, empty argument, trailing characters after a short
   *  option, or a token following a reserved / action token. */
  VL_ERROR_UNEXPECTED_ARGUMENT,
  VL_ERROR_DUPLICATE_OPTION,        /**< repeated with VL_OPTION_REPEAT_ERROR */
  VL_ERROR_OUT_OF_MEMORY,           /**< allocation failed during parse */
  VL_ERROR_MISSING_COMMAND,         /**< verb / sub-verb expected but absent */
  VL_ERROR_UNKNOWN_COMMAND,         /**< verb / sub-verb token not declared */
  VL_ERROR_MISSING_REQUIRED,        /**< `.required` option never provided */
  VL_ERROR_CONFLICTING_OPTION,      /**< two `.conflicts` options both present */
  VL_ERROR_UNSATISFIED_REQUIREMENT, /**< a `.requires` option is absent */
} vl_error_code_t;

/** One parse error. Strings are parser-owned. */
typedef struct vl_error {
  /** argv index of the offending token; 0 for post-parse checks such as
   *  VL_ERROR_MISSING_REQUIRED. */
  int argv_index;
  const char *key;      /**< option / verb name involved, or NULL */
  vl_error_code_t code; /**< classification */
  const char *message;  /**< human-readable detail, or NULL */
} vl_error_t;

/** Maximum segments in a dotted path (`verb.subverb.option`, `group.leaf`). */
enum { VL_PATH_MAX_SEGMENTS = 4 };

/** Split view of a dotted path. Segment pointers reference caller scratch. */
typedef struct vl_path {
  const char *segments[VL_PATH_MAX_SEGMENTS]; /**< NUL-terminated segments */
  size_t count;                               /**< segments filled */
} vl_path_t;

/** Split `dotted` (e.g. "agent.list") into segments. Copies `dotted` into
 *  `scratch` (dots become NUL) and stores segment pointers into it; performs
 *  no allocation.
 *
 *  @param dotted      Path to split; NULL or "" yields 0.
 *  @param scratch     Caller buffer that receives the copy and backs the
 *                     segment pointers in `out`; must outlive `out`.
 *  @param scratch_len Size of `scratch` in bytes, including the NUL.
 *  @param out         Receives segment pointers and count.
 *  @return The segment count, or 0 on empty input, an empty segment
 *          (leading/trailing/double dot), more than VL_PATH_MAX_SEGMENTS
 *          segments, or scratch too small to hold `dotted`. */
size_t vl_path_split(const char *dotted, char *scratch, size_t scratch_len, vl_path_t *out);

/** Kind of node a `--help=<target>` resolved to. */
typedef enum vl_help_kind {
  VL_HELP_NONE = 0, /**< no match */
  VL_HELP_VERB,     /**< a top-level verb */
  VL_HELP_SUBVERB,  /**< a sub-verb under `verb` */
  VL_HELP_OPTION,   /**< a global, verb or sub-verb option */
  VL_HELP_GROUP,    /**< a dotted `group.` prefix shared by options */
} vl_help_kind_t;

/** A resolved help target and its owning chain. Pointers reference the
 *  caller-supplied verb table / option arrays (not copied); `group` points
 *  into the caller's `target` string. Absent fields are NULL. */
typedef struct vl_help_resolution {
  vl_help_kind_t kind;       /**< what `target` resolved to */
  const vl_verb_t *verb;     /**< owning verb (VERB / SUBVERB / OPTION / a
                              *   verb-scoped GROUP; NULL for a bare GROUP) */
  const vl_verb_t *subverb;  /**< owning sub-verb (SUBVERB / nested OPTION or
                              *   GROUP) */
  const vl_option_t *option; /**< matched option (OPTION) */
  const char *group;         /**< group prefix (GROUP), e.g. "proxy" */
} vl_help_resolution_t;

/** Resolve a (possibly dotted) `--help=<target>` against a verb table and
 *  global options. Pure and allocation-free.
 *
 *  Structural forms win first, innermost scope before outermost: `verb`, a
 *  unique `subverb`, `verb.subverb`, then an option under the matched
 *  sub-verb, then one under the matched verb. Every segment after the matched
 *  verb / sub-verb is rejoined with dots to form the option name, so a dotted
 *  option name reaches its owner (`verb.subverb.group.leaf` matches
 *  `.name = "group.leaf"`, `verb.group.leaf` matches an option on the verb).
 *  A trailing single segment that is not an option name resolves as a group
 *  prefix instead (`verb.proxy`, `verb.subverb.proxy`, or bare `proxy` when
 *  `proxy.*` options exist). An exact option name anywhere in the tree is the
 *  last resort, which is what matches a bare dotted name like `proxy.lane`.
 *
 *  Targets longer than 127 bytes, or with more than VL_PATH_MAX_SEGMENTS
 *  segments, skip structural resolution and match by exact name only.
 *
 *  @param verbs        Verb table (may be NULL).
 *  @param verb_count   Entries in `verbs`; 0 means NULL-terminated.
 *  @param globals      Global option table (may be NULL).
 *  @param global_count Entries in `globals`; 0 means NULL-terminated.
 *  @param target       Help target text without the `--help=` prefix.
 *  @param out          Filled on success; untouched otherwise.
 *  @return true and fills *out on a match, false otherwise. */
bool vl_help_resolve(const vl_verb_t *const *verbs, size_t verb_count, const vl_option_t *const *globals,
                     size_t global_count, const char *target, vl_help_resolution_t *out);

/** Validate and copy a schema into a new parser. NULL creates an empty parser.
 *  Also applies `settings->color` via vl_color_init().
 *
 *  @param settings Schema to validate and copy; may be NULL. Not retained,
 *                  but option `.data` targets and the `.conflicts` /
 *                  `.requires` tables it references must outlive the parser.
 *  @return A new parser, or NULL when the schema is invalid or allocation
 *          fails. Target pointers in option `.data` fields remain
 *          caller-owned. Release with vl_destroy(). */
valve_t *vl_create(const vl_executable_t *settings);

/** Release parser-owned state. Accepts NULL. Caller-owned targets are not
 *  cleared; use vl_targets_clear after application handlers finish.
 *
 *  @param v Parser from vl_create(), or NULL. */
void vl_destroy(valve_t *v);

/** Free an argv array and each entry. Use only for caller-allocated arrays,
 *  never for the argv supplied to main. Accepts a NULL argv.
 *
 *  @param argc Number of entries to free.
 *  @param argv Array allocated with malloc(); each entry is passed to free().
 */
void vl_argv_destroy(int argc, char **argv);

/** Parse argv, replacing prior results and errors on `v`.
 *
 *  argv[0] is the program name and is skipped. With no further tokens the
 *  help handler fires. When the active schema declares a command option,
 *  argv must be main-style and include `argv[argc] == NULL`; ordinary
 *  schemas remain argc-bounded and do not read that sentinel. Result/error
 *  pointers remain valid until the next vl_parse call or vl_destroy.
 *
 *  @param v    Parser; NULL returns -1 without recording an error.
 *  @param argc Token count, including argv[0]; negative returns -1.
 *  @param argv Token vector; NULL returns -1.
 *  @return 0 on success (including a reserved help/version request or a
 *          fired action) and -1 on invalid input or parse errors; inspect
 *          vl_error_at() / vl_errors_print() for details. */
int vl_parse(valve_t *v, int argc, char **argv);

/** @return true when the last vl_parse() stored a result under `key`
 *          (an option `.name`). NULL `v` or `key` yields false. */
bool vl_has(const valve_t *v, const char *key);

/** Return a parsed value by key, or NULL. The pointer belongs to `v` and is
 *  invalidated by the next vl_parse call or vl_destroy.
 *
 *  @param v   Parser; NULL yields NULL.
 *  @param key Option `.name` (also for short / toggle forms); NULL yields
 *             NULL.
 *  @return Parser-owned value, or NULL when the option was not given. */
const vl_value_t *vl_get(const valve_t *v, const char *key);

/** Release the heap held by a caller-owned value (raw text, KV pairs, array
 *  items, recursively) and zero it. Accepts NULL. Never call it on a value
 *  returned by vl_get() or vl_result_at(); use it for VL_TARGET_VALUE /
 *  VL_TARGET_TOGGLE targets (vl_targets_clear() does this for you).
 *
 *  @param value Value to clear; NULL is ignored. */
void vl_value_clear(vl_value_t *value);

/** @return Active verb name selected by the last vl_parse(), or NULL when
 *          the schema has no verbs, no verb was reached (reserved token or
 *          action fired at argv[1]) or `v` is NULL. Parser-owned. */
const char *vl_verb_get(const valve_t *v);

/** Active sub-verb name (second level), or NULL when none was selected. */
const char *vl_subverb_get(const valve_t *v);

/** @return true when the last vl_parse() was intercepted by a reserved
 *          token (--help / --version / --valve or an alias), by the
 *          no-argument help fallback, or by an executable action. When
 *          true the caller should normally exit without running its own
 *          logic. */
bool vl_reserved_fired(const valve_t *v);

/** @return The target given to the last help request (`--help=<target>` in
 *          inline mode, `--help <target>` in separate mode, or the joined
 *          `?target` form in either), or NULL when help fired without a
 *          target or did not fire. Parser-owned; meant for use inside an
 *          `on_help` handler together with vl_help_resolve(). */
const char *vl_help_target(const valve_t *v);

/** Name of the executable action that fired during the last vl_parse, or
 *  NULL when none (or a built-in reserved token) fired. Parser-owned. */
const char *vl_action_fired(const valve_t *v);

/** @return Number of stored results from the last vl_parse(); 0 for NULL. */
size_t vl_result_count(const valve_t *v);

/** Return a parser-owned result, or NULL when `index` is out of range.
 *
 *  @param v     Parser; NULL yields NULL.
 *  @param index Position in first-seen order, `< vl_result_count()`. */
const vl_result_t *vl_result_at(const valve_t *v, size_t index);

/** @return Number of errors recorded by the last vl_parse(); 0 for NULL. */
size_t vl_error_count(const valve_t *v);

/** Return a parser-owned error, or NULL when `index` is out of range.
 *
 *  @param v     Parser; NULL yields NULL.
 *  @param index Position in recording order, `< vl_error_count()`. */
const vl_error_t *vl_error_at(const valve_t *v, size_t index);

/** Visitor for vl_errors_foreach(). `err` is parser-owned and valid only
 *  during the call. */
typedef void (*vl_error_fn)(const vl_error_t *err, void *userdata);

/** Call `fn` for each parse error in order. No-op if v or fn is NULL.
 *
 *  @param v        Parser.
 *  @param fn       Visitor invoked once per error.
 *  @param userdata Passed through to `fn` untouched. */
void vl_errors_foreach(const valve_t *v, vl_error_fn fn, void *userdata);

/**
 * Print each error to `stream` (stderr when stream is NULL):
 *   error argv[<i>] [<key>]: <message>
 * Plain text only (no color). Custom sinks use vl_errors_foreach.
 *
 * @param v      Parser; NULL prints nothing.
 * @param stream Destination, or NULL for stderr.
 */
void vl_errors_print(const valve_t *v, FILE *stream);

/**
 * Walk `settings` option tables (executable globals, every verb/subverb
 * options, and the actions table when VL_BEHAVIOR_ACCEPT_NO_VERB is set)
 * and clear Valve-owned target memory:
 *   VL_TARGET_STRING              → free(*(char **)ptr); *ptr = NULL
 *   VL_TARGET_VALUE / TOGGLE      → vl_value_clear
 *   VL_TARGET_COMMAND             → zero the borrowed command view
 *   VL_TARGET_INT / INT64 / DOUBLE / BOOL → zero
 * Idempotent. Does not free `settings` itself or any valve_t.
 *
 * Call after handlers — not inside vl_destroy. Clear ≠ destroy: targets on
 * the settings struct must remain live until the caller releases them.
 * Only clears targets reachable from the vl_executable_t option graph.
 * Non-Valve heap in the same structs remains the caller's problem.
 *
 * @param settings The same schema passed to vl_create(); NULL is a no-op.
 */
void vl_targets_clear(const vl_executable_t *settings);

/** @return The library version as a static string (e.g. "1.2.0"). */
const char *vl_version_get(void);

/** @return Static banner used by the default `--valve` printer: the Valve
 *          glyph followed by `valve v<version>`. */
const char *vl_version_string(void);

#endif /* VALVE_H */
