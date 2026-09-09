#ifndef VALVE_PRIVATE_H
#define VALVE_PRIVATE_H

#ifndef VALVE_VERSION
#define VALVE_VERSION "1.1.0-002"
#endif

#define PROGRAM_VERSION "1.1.0-002"
#define PROGRAM_NAME "program"

#include "valve/valve.h"

typedef struct valve_verb valve_verb_t;
typedef struct valve_conflict valve_conflict_t;
typedef struct valve_requirement valve_requirement_t;

typedef enum {
  VL_RESERVED_NONE = 0,
  VL_RESERVED_VALVE,
  VL_RESERVED_HELP,
  VL_RESERVED_VERSION,
} vl_reserved_kind_t;

struct valve {

  vl_assign_t assign_;
  vl_behavior_t behavior_;

  const vl_metadata_t metadata;
  char *program_name_;
  char *program_version_;
  char *description_;
  char *usage_;
  char *logo_;

  valve_verb_t *verbs_;
  size_t verb_count_;
  const valve_verb_t *active_verb_;
  const valve_verb_t *active_subverb_;

  vl_option_t *options_;
  size_t option_count_;

  valve_conflict_t *conflicts_;
  size_t conflict_count_;
  size_t conflict_cap_;

  valve_requirement_t *requirements_;
  size_t requirement_count_;
  size_t requirement_cap_;

  void (*on_help_)(const valve_t *v);
  void (*on_version_)(const valve_t *v);
  void (*on_valve_)(const valve_t *v);

  char *help_target_;

  bool parsed_;
  bool reserved_fired_;

  // down from here useful for the caller
  vl_result_t *results_;
  size_t result_count_;
  size_t result_cap_;

  vl_error_t *errors_;
  size_t error_count_;
  size_t error_cap_;
};

typedef struct valve_verb {
  char *name;
  vl_option_t *options;
  size_t option_count;
  char *description;
  char *usage;
  struct valve_verb *verbs; /* sub-verbs, copied recursively */
  size_t verb_count;
} valve_verb_t;

struct valve_conflict {
  const vl_option_t *first;
  const vl_option_t *second;
  const valve_verb_t *verb;
  const valve_verb_t *subverb;
};

struct valve_requirement {
  const vl_option_t *source;
  const vl_option_t *target;
  const valve_verb_t *verb;
  const valve_verb_t *subverb;
};


// verb
size_t verb_count_(const vl_executable_t *settings);
size_t verb_table_count_(const vl_verb_t *const *verbs, size_t verb_count);
int verbs_copy_(const vl_verb_t *const *src, size_t count, valve_verb_t **dst,
                size_t *dst_count);
const valve_verb_t *vl_verb_find_(const valve_t *v, const char *name);
const valve_verb_t *verb_find_in_(const valve_verb_t *verbs, size_t count,
                                  const char *name);
void vl_verbs_clear_(valve_t *v);
void verb_array_clear_(valve_verb_t *verbs, size_t count);
bool verb_name_valid_(const char *name);
bool verbs_use_reserved_(const vl_verb_t *const *verbs, size_t count);
bool verbs_have_duplicate_(const vl_verb_t *const *verbs, size_t count);
bool verbs_have_invalid_(const vl_verb_t *const *verbs, size_t count);

// option
void vl_options_clear_(valve_t *v);
int options_copy_(const vl_option_t *const *src, size_t count,
                         vl_option_t **dst, size_t *dst_count);
bool options_have_invalid_(const vl_option_t *const *options, size_t count);
bool options_use_reserved_(const vl_option_t *const *options, size_t count);
bool option_collides_with_globals_(const vl_option_t *const *globals,
                                          size_t global_count,
                                          const vl_option_t *const *locals,
                                          size_t local_count);
size_t option_count_(const vl_option_t *const *options, size_t option_count);
bool options_have_duplicate_long_(const vl_option_t *const *options, size_t count);
bool options_have_duplicate_short_(const vl_option_t *const *options, size_t count);
bool option_type_valid_(vl_opt_type_t style);
bool option_value_valid_(vl_option_value_t value);
bool option_repeat_valid_(vl_option_repeat_t repeat);
bool option_command_valid_(const vl_option_t *option);
size_t options_command_count_(const vl_option_t *const *options, size_t count);
void option_array_free_(vl_option_t *options, size_t count);
bool option_name_valid_(const char *name);
size_t option_conflict_count_(const vl_option_t *option);
size_t option_requirement_count_(const vl_option_t *option);
char *option_label_(const vl_option_t *option);
void option_label_print_(const vl_option_t *option, bool comma);
bool option_in_(const vl_option_t *option,
                const vl_option_t *const *options, size_t count);
bool option_visible_(const vl_option_t *option,
                     const vl_option_t *const *globals, size_t global_count,
                     const vl_option_t *const *parents, size_t parent_count,
                     const vl_option_t *const *options, size_t count);
const char *option_value_label_(const vl_option_t *option);
/** Outcome of parsing a VL_OPTION_VALUE_TIME literal. Each failure is distinct
 *  so the parser can say what was wrong rather than only that it was wrong. */
typedef enum option_duration_status {
  OPTION_DURATION_OK = 0,
  OPTION_DURATION_EMPTY,
  OPTION_DURATION_NEGATIVE,
  OPTION_DURATION_SUFFIX,
  OPTION_DURATION_SYNTAX,
  OPTION_DURATION_OVERFLOW,
} option_duration_status_t;
option_duration_status_t option_duration_parse_(const char *raw, int64_t *out);
const char *option_duration_message_(option_duration_status_t status);
void option_usage_print_(const valve_t *v, const vl_option_t *option);
void option_annotations_print_(const valve_t *v,
                               const vl_option_t *option);
bool options_conflicts_valid_(const vl_option_t *const *globals,
                              size_t global_count,
                              const vl_option_t *const *parents,
                              size_t parent_count,
                              const vl_option_t *const *options, size_t count);
bool options_requirements_valid_(const vl_option_t *const *globals,
                                 size_t global_count,
                                 const vl_option_t *const *parents,
                                 size_t parent_count,
                                 const vl_option_t *const *options,
                                 size_t count);
const vl_option_t *option_mapped_(
    const vl_option_t *source, const vl_option_t *const *source_globals,
    const vl_option_t *copied_globals, size_t global_count,
    const vl_option_t *const *source_parents,
    const vl_option_t *copied_parents, size_t parent_count,
    const vl_option_t *const *source_options,
    const vl_option_t *copied_options, size_t option_count);

// conflict
int vl_conflicts_copy_(valve_t *v, const vl_executable_t *settings);
void vl_conflicts_clear_(valve_t *v);
int vl_conflicts_check_(valve_t *v);
bool vl_conflict_active_(const valve_t *v,
                         const valve_conflict_t *conflict);
const vl_option_t *vl_conflict_other_(const valve_conflict_t *conflict,
                                      const vl_option_t *option);
bool vl_conflict_seen_(const valve_t *v, size_t before,
                       const vl_option_t *option,
                       const vl_option_t *other);
int vl_conflict_error_(valve_t *v, const vl_option_t *later,
                       const vl_result_t *later_result,
                       const vl_option_t *earlier);
bool vl_conflict_exists_(const valve_t *v, const vl_option_t *first,
                         const vl_option_t *second,
                         const valve_verb_t *verb,
                         const valve_verb_t *subverb);
int vl_conflict_add_(valve_t *v, const vl_option_t *first,
                     const vl_option_t *second, const valve_verb_t *verb,
                     const valve_verb_t *subverb);
int vl_conflicts_copy_table_(
    valve_t *v, const vl_option_t *const *source_globals,
    const vl_option_t *copied_globals, size_t global_count,
    const vl_option_t *const *source_parents,
    const vl_option_t *copied_parents, size_t parent_count,
    const vl_option_t *const *source_options,
    const vl_option_t *copied_options, size_t option_count,
    const valve_verb_t *verb, const valve_verb_t *subverb);

// requirement
int vl_requirements_copy_(valve_t *v, const vl_executable_t *settings);
void vl_requirements_clear_(valve_t *v);
int vl_requirements_check_(valve_t *v);
bool vl_requirement_active_(const valve_t *v,
                            const valve_requirement_t *requirement);
bool vl_requirement_exists_(const valve_t *v,
                            const vl_option_t *source,
                            const vl_option_t *target,
                            const valve_verb_t *verb,
                            const valve_verb_t *subverb);
int vl_requirement_add_(valve_t *v, const vl_option_t *source,
                        const vl_option_t *target,
                        const valve_verb_t *verb,
                        const valve_verb_t *subverb);
int vl_requirements_copy_table_(
    valve_t *v, const vl_option_t *const *source_globals,
    const vl_option_t *copied_globals, size_t global_count,
    const vl_option_t *const *source_parents,
    const vl_option_t *copied_parents, size_t parent_count,
    const vl_option_t *const *source_options,
    const vl_option_t *copied_options, size_t option_count,
    const valve_verb_t *verb, const valve_verb_t *subverb);
int vl_requirement_error_(valve_t *v, const vl_option_t *source,
                          const vl_result_t *source_result,
                          const vl_option_t *target);
bool vl_requirement_forward_seen_(const valve_t *v, size_t before,
                                  const vl_option_t *source,
                                  const vl_option_t *target);
bool vl_requirement_reverse_seen_(const valve_t *v, size_t before,
                                  const vl_option_t *target,
                                  const vl_option_t *source);

// schema
bool vl_schema_validate_(const vl_executable_t *settings);

// refactor down all
bool assign_valid_(vl_assign_t assign);

bool target_valid_(vl_target_t target);
bool short_name_valid_(char short_name);

void vl_results_clear_(valve_t *v);
void vl_errors_clear_(valve_t *v);

void vl_settings_meta_clear_(valve_t *v);

int vl_error_add_(valve_t *v, vl_error_code_t code, int argv_index,
                  const char *key, const char *message);

const vl_option_t *vl_option_find_(const valve_t *v, const char *name);
const vl_option_t *vl_option_find_n_(const valve_t *v, const char *name,
                                     size_t len);
const vl_option_t *vl_option_find_short_(const valve_t *v, char short_name);
const vl_option_t *vl_option_find_command_(const valve_t *v);

int vl_parse_command_(valve_t *v, int argc, char **argv, int argv_index);

int vl_result_set_(valve_t *v, const vl_option_t *opt, vl_value_t *value,
                   int argv_index);
const vl_result_t *vl_result_find_(const valve_t *v, const char *name);

int vl_required_check_(valve_t *v);

vl_reserved_kind_t vl_reserved_kind_(const char *token);
vl_reserved_kind_t vl_reserved_name_(const char *name);
vl_reserved_kind_t vl_reserved_short_(char c);

/** Internal counterpart of vl_help_resolution_t over the parser's copied
 *  verb tree (valve_verb_t). Used by parse.c validation and the default
 *  renderer so both agree on what a `--help=<target>` resolves to. */
typedef struct vl_help_internal {
  vl_help_kind_t kind;
  const valve_verb_t *verb;
  const valve_verb_t *subverb;
  const vl_option_t *option;
  const char *group; /** points into the caller's target string */
} vl_help_internal_t;

bool vl_help_resolve_internal_(const valve_t *v, const char *target,
                               vl_help_internal_t *out);

void vl_help_print_default_(const valve_t *v);
void vl_version_print_default_(const valve_t *v);
void vl_valve_print_default_(const valve_t *v);

#endif /* VALVE_PRIVATE_H */
