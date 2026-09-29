#ifndef VALVE_PARSE_PRIVATE_H
#define VALVE_PARSE_PRIVATE_H

#include "../../private.h"

/* Syntactic form a reserved token (--help / --version / --valve) arrived in.
 * Only help accepts a target; the form decides where that target lives. */
typedef enum vl_parse_reserved_form {
  VL_PARSE_RESERVED_BARE = 0, /* --help, -h, ?          (target may follow) */
  VL_PARSE_RESERVED_EQ,       /* --help=<target>        (inline mode only)  */
  VL_PARSE_RESERVED_JOINED,   /* ?<target>, -?<target>  (any mode)          */
} vl_parse_reserved_form_t;

/* Unit return convention shared by the staged helpers below: 1 means "not
 * mine, keep going", 0 means "handled, vl_parse returns 0", -1 means an
 * error was recorded and vl_parse returns -1. */
enum { VL_PARSE_CONTINUE = 1 };

// token classification
bool vl_parse_has_style_(const vl_option_t *opt, vl_opt_type_t style);
bool vl_parse_looks_like_option_(const char *s);
bool vl_parse_looks_like_negative_number_(const char *s);
bool vl_parse_is_enable_disable_(const char *arg, const char **name,
                                 bool *enabled);
const char *vl_parse_option_name_in_token_(const char *token,
                                           size_t *name_len);
bool vl_parse_user_has_option_for_token_(const valve_t *v, const char *token);
bool vl_parse_token_has_double_mark_(const char *raw);

// option-argument shape
vl_option_value_t vl_parse_effective_scalar_value_(const vl_option_t *opt);
bool vl_parse_accepts_numeric_value_(const vl_option_t *opt);
bool vl_parse_consumable_next_value_(int argc, char **argv, int index,
                                     const vl_option_t *opt);

// literals
bool vl_parse_int_literal_(const char *raw, int64_t *out);
bool vl_parse_double_literal_(const char *raw, double *out);
int vl_parse_check_int_bounds_(valve_t *v, const vl_option_t *opt,
                               int64_t value, int argv_index);

// value production (each stores through vl_result_set_)
int vl_parse_bool_value_(valve_t *v, const vl_option_t *opt, bool enabled,
                         int argv_index);
int vl_parse_scalar_value_(valve_t *v, const vl_option_t *opt,
                           const char *raw, int argv_index);
int vl_parse_kv_value_(valve_t *v, const vl_option_t *opt, const char *raw,
                       int argv_index);
int vl_parse_array_element_(const char **cursor, vl_value_t *value,
                            const char **message);
int vl_parse_array_value_(valve_t *v, const vl_option_t *opt, const char *raw,
                          int argv_index);
int vl_parse_assign_value_(valve_t *v, const vl_option_t *opt,
                           const char *raw, int argv_index);

// option forms
int vl_parse_enable_disable_(valve_t *v, const char *arg, int argv_index);
int vl_parse_long_option_(valve_t *v, int argc, char **argv, int *index);
int vl_parse_short_option_(valve_t *v, int argc, char **argv, int *index);
int vl_parse_command_(valve_t *v, int argc, char **argv, int argv_index);

// reserved tokens
vl_reserved_kind_t vl_parse_reserved_token_(const char *token,
                                            vl_parse_reserved_form_t *form,
                                            const char **target);
int vl_parse_extract_reserved_target_(valve_t *v, vl_reserved_kind_t kind,
                                      vl_parse_reserved_form_t form, int argc,
                                      char **argv, int i,
                                      const char *target_in,
                                      const char **target_out,
                                      bool *consumed_extra);
const char *vl_parse_unknown_help_message_(const valve_t *v,
                                           const char *target, char *buf,
                                           size_t buflen);
void vl_parse_dispatch_reserved_(valve_t *v, vl_reserved_kind_t kind);
int vl_parse_dispatch_reserved_target_(valve_t *v, vl_reserved_kind_t kind,
                                       const char *target, int argv_index);
int vl_parse_try_reserved_(valve_t *v, int argc, char **argv, int i,
                           bool check_user_option);

// stages of vl_parse
int vl_parse_action_(valve_t *v, int argc, char **argv);
int vl_parse_select_verb_(valve_t *v, int argc, char **argv, int *start);
int vl_parse_select_subverb_(valve_t *v, int argc, char **argv, int *start);
int vl_parse_options_(valve_t *v, int argc, char **argv, int start);
int vl_parse_finish_(valve_t *v);

#endif /* VALVE_PARSE_PRIVATE_H */
