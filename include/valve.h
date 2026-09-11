#ifndef VALVE_H
#define VALVE_H

#include "color.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct valve valve_t;
typedef struct vl_metadata vl_metadata_t;
typedef struct vl_option vl_option_t;

typedef struct vl_verb vl_verb_t;

typedef enum vl_behavior {
  /** When set, vl_create accepts user options/verbs using reserved names
   *  (--help, --version, --valve and their aliases). At parse time, a
   *  user-defined option/verb wins over the reserved interception. */
  VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED = 1u << 0,
  /** Reserved for future operand support. This flag is not implemented and
   *  currently has no effect. vl_parse rejects ordinary positional tokens;
   *  `--` stops parsing and the remaining argv tail is ignored. */
  VL_BEHAVIOR_ACCEPT_OPERANDS = 1u << 1,
} vl_behavior_t;

typedef enum vl_assign {
  VL_ASSIGN_INLINE = 0, /** --flag=value, -f=value */
  VL_ASSIGN_SEPARATE,   /** --flag value, -f value */
} vl_assign_t;

struct vl_metadata {
  const char *program_name;
  const char *program_version;
  const char *description;
  const char *usage;
};

typedef struct vl_executable {

  vl_assign_t assign;
  vl_behavior_t behavior;

  /** Reserved migration aggregate. vl_create does not read this field in
   *  experimental 1.x; populate the direct metadata fields below. */
  const vl_metadata_t metadata;
  /** Preferred program metadata fields for experimental 1.x. Strings are
   *  copied by vl_create and may be released by the caller afterward. */
  const char *program_name;
  const char *program_version;
  const char *description;
  const char *usage;

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

  const vl_verb_t *const *verbs;
  size_t verb_count;

  const vl_option_t *const *options;
  size_t option_count;

  void (*on_help)(const valve_t *v);
  void (*on_version)(const valve_t *v);
  void (*on_valve)(const valve_t *v);

} vl_executable_t;

typedef struct vl_verb {
  const char *name;
  const char *description;
  const char *usage;
  const vl_option_t *const *options;
  size_t option_count;
  /** Optional nested verb table (sub-verbs): `program verb subverb ...`.
   *  When present, a sub-verb token is mandatory after the verb. Option
   *  lookup covers sub-verb options, then the owning verb's, then globals.
   *  The active names are read with vl_verb_get() / vl_subverb_get(). */
  const vl_verb_t *const *verbs;
  size_t verb_count;
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
  VL_OPT_TYPE_LONG   = 1u << 0, /** --flag */
  VL_OPT_TYPE_SHORT  = 1u << 1, /** -f */
  VL_OPT_TYPE_TOGGLE = 1u << 2, /** --enable-flag / --disable-flag */
} vl_opt_type_t;

typedef enum vl_option_value {
  VL_OPTION_VALUE_AUTO = 0,
  VL_OPTION_VALUE_STRING,
  VL_OPTION_VALUE_INT,
  VL_OPTION_VALUE_DOUBLE,
  VL_OPTION_VALUE_NUMBER,
  VL_OPTION_VALUE_KV,
  VL_OPTION_VALUE_TOGGLE,
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

typedef enum vl_option_repeat {
  VL_OPTION_REPEAT_ERROR = 0,
  VL_OPTION_REPEAT_ARRAY,
} vl_option_repeat_t;

typedef enum vl_target {
  VL_TARGET_NONE = 0,
  VL_TARGET_STRING,
  VL_TARGET_INT,
  VL_TARGET_INT64,
  VL_TARGET_DOUBLE,
  VL_TARGET_BOOL,
  VL_TARGET_VALUE,
  VL_TARGET_TOGGLE,
  VL_TARGET_COMMAND,
} vl_target_t;

typedef struct vl_option {
  const char *name;
  char short_name;
  const char *toggle_ref;
  const char *description;
  /** Optional usage guidance printed below Valve's inferred option syntax. */
  const char *usage;
  vl_opt_type_t type;
  vl_option_value_t value;
  vl_option_repeat_t repeat;
  /** When true, vl_parse() fails with VL_ERROR_MISSING_REQUIRED if the
   *  option was never provided. Checked for globals and the active verb
   *  chain; skipped when a reserved token (--help/--version/--valve) fires. */
  bool required;
  /** Options that cannot appear with this option. One declaration establishes
   *  the relationship in both directions for parsing and default help. A
   *  count of 0 means the table is NULL-terminated. */
  const vl_option_t *const *conflicts;
  size_t conflict_count;
  /** Options that must also appear when this option appears. The relationship
   *  is directed. A count of 0 means the table is NULL-terminated. */
  const vl_option_t *const *requires;
  size_t require_count;
  void *data;
  size_t offset;
  vl_target_t target;
  bool has_int_min;
  bool has_int_max;
  int64_t int_min;
  int64_t int_max;
} vl_option_t;

typedef struct vl_kv_pair vl_kv_pair_t;
typedef struct vl_value vl_value_t;

typedef struct vl_kv_list {
  const vl_kv_pair_t *pairs;
  size_t count;
} vl_kv_list_t;

typedef struct vl_array {
  const vl_value_t *items;
  size_t count;
} vl_array_t;

/** Borrowed command tail captured after `--`.
 *
 * `argv` points into the vector passed to vl_parse() and is never copied or
 * freed by Valve. A schema declaring VL_OPTION_VALUE_COMMAND requires a
 * main-style input vector with `argv[argc] == NULL`, which makes this slice
 * directly suitable for execvp(). The strings and vector must outlive every
 * use of this value. */
typedef struct vl_command {
  char *const *argv;
  int argc;
} vl_command_t;

typedef enum vl_value_kind {
  VL_VALUE_STRING = 0,
  VL_VALUE_INT,
  VL_VALUE_DOUBLE,
  VL_VALUE_BOOL,
  VL_VALUE_KV,
  VL_VALUE_ARRAY,
  VL_VALUE_COMMAND,
} vl_value_kind_t;

struct vl_value {
  vl_value_kind_t kind;
  const char *raw;
  union {
    int64_t integer;
    double number;
    bool boolean;
    vl_kv_list_t kv;
    vl_array_t array;
    vl_command_t command;
  } as;
};

struct vl_kv_pair {
  const char *key;
  vl_value_t value;
};

typedef struct vl_result {
  const char *key;
  vl_value_t value;
  int argv_index;
} vl_result_t;

typedef enum vl_error_code {
  VL_ERROR_UNKNOWN_OPTION = 1,
  VL_ERROR_DISABLED_FORM,
  VL_ERROR_MISSING_VALUE,
  VL_ERROR_INVALID_VALUE,
  VL_ERROR_UNEXPECTED_ARGUMENT,
  VL_ERROR_DUPLICATE_OPTION,
  VL_ERROR_OUT_OF_MEMORY,
  VL_ERROR_MISSING_COMMAND,
  VL_ERROR_UNKNOWN_COMMAND,
  VL_ERROR_MISSING_REQUIRED,
  VL_ERROR_CONFLICTING_OPTION,
  VL_ERROR_UNSATISFIED_REQUIREMENT,
} vl_error_code_t;

typedef struct vl_error {
  int argv_index;
  const char *key;
  vl_error_code_t code;
  const char *message;
} vl_error_t;

/** Maximum segments in a dotted path (`verb.subverb.option`, `group.leaf`). */
enum { VL_PATH_MAX_SEGMENTS = 4 };

/** Split view of a dotted path. Segment pointers reference caller scratch. */
typedef struct vl_path {
  const char *segments[VL_PATH_MAX_SEGMENTS];
  size_t count;
} vl_path_t;

/** Split `dotted` (e.g. "agent.list") into segments. Copies `dotted` into
 *  `scratch` (dots become NUL) and stores segment pointers into it; performs
 *  no allocation. Returns the segment count, or 0 on empty input, an empty
 *  segment (leading/trailing/double dot), more than VL_PATH_MAX_SEGMENTS
 *  segments, or scratch too small to hold `dotted`. */
size_t vl_path_split(const char *dotted, char *scratch, size_t scratch_len,
                     vl_path_t *out);

/** Kind of node a `--help=<target>` resolved to. */
typedef enum vl_help_kind {
  VL_HELP_NONE = 0,
  VL_HELP_VERB,
  VL_HELP_SUBVERB,
  VL_HELP_OPTION,
  VL_HELP_GROUP,
} vl_help_kind_t;

/** A resolved help target and its owning chain. Pointers reference the
 *  caller-supplied verb table / option arrays (not copied); `group` points
 *  into the caller's `target` string. Absent fields are NULL. */
typedef struct vl_help_resolution {
  vl_help_kind_t kind;
  const vl_verb_t *verb;     /** owning verb (VERB / SUBVERB / OPTION / a
                              *  verb-scoped GROUP; NULL for a bare GROUP) */
  const vl_verb_t *subverb;  /** owning sub-verb (SUBVERB / nested OPTION or
                              *  GROUP) */
  const vl_option_t *option; /** matched option (OPTION) */
  const char *group;         /** group prefix (GROUP), e.g. "proxy" */
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
 *  A count of 0 means the array is NULL-terminated. Returns true and fills
 *  *out on a match. */
bool vl_help_resolve(const vl_verb_t *const *verbs, size_t verb_count,
                     const vl_option_t *const *globals, size_t global_count,
                     const char *target, vl_help_resolution_t *out);

/** Validate and copy a schema into a new parser. NULL creates an empty parser.
 *  Returns NULL when the schema is invalid or allocation fails. Target
 *  pointers in option `.data` fields remain caller-owned. */
valve_t *vl_create(const vl_executable_t *settings);

/** Release parser-owned state. Accepts NULL. Caller-owned targets are not
 *  cleared; use vl_targets_clear after application handlers finish. */
void vl_destroy(valve_t *v);

/** Free an argv array and each entry. Use only for caller-allocated arrays,
 *  never for the argv supplied to main. Accepts a NULL argv. */
void vl_argv_destroy(int argc, char **argv);

/** Parse argv, replacing prior results and errors on `v`. Returns 0 on
 *  success (including a reserved help/version request) and -1 on invalid
 *  input or parse errors. Result/error pointers remain valid until the next
 *  vl_parse call or vl_destroy. When the active schema declares a command
 *  option, argv must be main-style and include `argv[argc] == NULL`; ordinary
 *  schemas remain argc-bounded and do not read that sentinel. */
int vl_parse(valve_t *v, int argc, char **argv);

bool vl_has(const valve_t *v, const char *key);

/** Return a parsed value by key, or NULL. The pointer belongs to `v` and is
 *  invalidated by the next vl_parse call or vl_destroy. */
const vl_value_t *vl_get(const valve_t *v, const char *key);

void vl_value_clear(vl_value_t *value);

const char *vl_verb_get(const valve_t *v);

/** Active sub-verb name (second level), or NULL when none was selected. */
const char *vl_subverb_get(const valve_t *v);

bool vl_reserved_fired(const valve_t *v);

const char *vl_help_target(const valve_t *v);

size_t vl_result_count(const valve_t *v);

/** Return a parser-owned result, or NULL when `index` is out of range. */
const vl_result_t *vl_result_at(const valve_t *v, size_t index);

size_t vl_error_count(const valve_t *v);

/** Return a parser-owned error, or NULL when `index` is out of range. */
const vl_error_t *vl_error_at(const valve_t *v, size_t index);

typedef void (*vl_error_fn)(const vl_error_t *err, void *userdata);

/** Call `fn` for each parse error in order. No-op if v or fn is NULL. */
void vl_errors_foreach(const valve_t *v, vl_error_fn fn, void *userdata);

/**
 * Print each error to `stream` (stderr when stream is NULL):
 *   error argv[<i>] [<key>]: <message>
 * Plain text only (no color). Custom sinks use vl_errors_foreach.
 */
void vl_errors_print(const valve_t *v, FILE *stream);

/**
 * Walk `settings` option tables (executable globals + every verb/subverb
 * options) and clear Valve-owned target memory:
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
 */
void vl_targets_clear(const vl_executable_t *settings);

const char *vl_version_get(void);

const char *vl_version_string(void);

#endif /* VALVE_H */
