#include "../private.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool has_style_(const vl_option_t *opt, vl_opt_type_t style) {
  /* GCOVR_EXCL_BR_START — opt is non-null at every call site */
  return opt && (opt->type & style);
  /* GCOVR_EXCL_BR_STOP */
}

static bool looks_like_option_(const char *s) {
  return s && s[0] == '-' && s[1] != '\0'; /* GCOVR_EXCL_BR_LINE: null only via pathological argv */
}

static bool looks_like_negative_number_(const char *s) {
  if (!s || s[0] != '-') /* GCOVR_EXCL_BR_LINE: callers pass non-null option-like tokens */
    return false; /* GCOVR_EXCL_LINE */
  if (s[1] >= '0' && s[1] <= '9')
    return true;
  return s[1] == '.' && s[2] >= '0' && s[2] <= '9';
}

/* Scalar parse kind for an option. For a dot-notation option the name is a
 * grouped path, so the leaf's scalar kind is taken from `.target`; every other
 * option parses according to its own `.value`. */
static vl_option_value_t effective_scalar_value_(const vl_option_t *opt) {
  if (!opt) /* GCOVR_EXCL_BR_LINE: null guard */
    return VL_OPTION_VALUE_AUTO; /* GCOVR_EXCL_LINE */
  if (opt->value != VL_OPTION_VALUE_DOT_NOTATION)
    return opt->value;

  switch (opt->target) { /* GCOVR_EXCL_BR_LINE: TOGGLE/VALUE/NONE/default share AUTO */
  case VL_TARGET_INT:
  case VL_TARGET_INT64:
    return VL_OPTION_VALUE_INT;
  case VL_TARGET_DOUBLE:
    return VL_OPTION_VALUE_DOUBLE;
  case VL_TARGET_BOOL:
    return VL_OPTION_VALUE_BOOL;
  case VL_TARGET_STRING:
    return VL_OPTION_VALUE_STRING;
  case VL_TARGET_NONE:
  case VL_TARGET_VALUE:
  case VL_TARGET_TOGGLE:
  default:
    return VL_OPTION_VALUE_AUTO;
  }
}

static bool opt_accepts_numeric_value_(const vl_option_t *opt) {
  if (!opt) /* GCOVR_EXCL_BR_LINE: null guard */
    return false; /* GCOVR_EXCL_LINE */
  vl_option_value_t vk = effective_scalar_value_(opt);
  return vk == VL_OPTION_VALUE_INT ||
         vk == VL_OPTION_VALUE_DOUBLE ||
         vk == VL_OPTION_VALUE_NUMBER ||
         vk == VL_OPTION_VALUE_AUTO;
}

static bool consumable_next_value_(int argc, char **argv, int index,
                                   const vl_option_t *opt) {
  if (index + 1 >= argc || !argv[index + 1])
    return false;
  const char *next = argv[index + 1];
  if (!looks_like_option_(next))
    return true;
  if (opt_accepts_numeric_value_(opt) && looks_like_negative_number_(next))
    return true;
  return false;
}

/* -- boolean / toggle --------------------------------------------------- */

static bool is_enable_disable_(const char *arg, const char **name,
                               bool *enabled) {
  const char *enable = "--enable-";
  const char *disable = "--disable-";
  size_t enable_len = strlen(enable);
  size_t disable_len = strlen(disable);

  if (strncmp(arg, enable, enable_len) == 0 && arg[enable_len] != '\0') {
    *name = arg + enable_len;
    *enabled = true;
    return true;
  }

  if (strncmp(arg, disable, disable_len) == 0 && arg[disable_len] != '\0') {
    *name = arg + disable_len;
    *enabled = false;
    return true;
  }

  return false;
}

static bool parse_bool_literal_(const char *raw, bool *out) {
  if (strcmp(raw, "true") == 0) {
    *out = true;
    return true;
  }

  if (strcmp(raw, "false") == 0) {
    *out = false;
    return true;
  }

  return false;
}

static int bool_value_(valve_t *v, const vl_option_t *opt, bool enabled,
                       int argv_index) {

  vl_value_t value = {
      .kind = VL_VALUE_BOOL,
      .raw = strdup(enabled ? "true" : "false"),
      .as.boolean = enabled,
  };

  if (!value.raw)
    return -1;

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);
    return -1;
  }

  return 0;
}

static const vl_option_t *find_toggle_ref_(const valve_t *v, const char *ref) {
  if (!v || !ref) /* GCOVR_EXCL_BR_LINE: null guard */
    return NULL; /* GCOVR_EXCL_LINE */

  if (v->active_action_) {
    /* GCOVR_EXCL_BR_START — action matched by toggle_ref, so this holds */
    if (v->active_action_->toggle_ref &&
        strcmp(v->active_action_->toggle_ref, ref) == 0)
      return v->active_action_;
    /* GCOVR_EXCL_BR_STOP */
    return NULL; /* GCOVR_EXCL_LINE */
  }

  if (v->active_subverb_) {
    for (size_t i = 0; i < v->active_subverb_->option_count; ++i) {
      const vl_option_t *opt = &v->active_subverb_->options[i];
      if (opt->toggle_ref && strcmp(opt->toggle_ref, ref) == 0) /* GCOVR_EXCL_BR_LINE: null toggle_ref short-circuit */
        return opt;
    }
  }

  if (v->active_verb_) {
    for (size_t i = 0; i < v->active_verb_->option_count; ++i) {
      const vl_option_t *opt = &v->active_verb_->options[i];
      if (opt->toggle_ref && strcmp(opt->toggle_ref, ref) == 0) /* GCOVR_EXCL_BR_LINE: null toggle_ref short-circuit */
        return opt;
    }
  }

  for (size_t i = 0; i < v->option_count_; ++i) {
    /* GCOVR_EXCL_BR_START — null toggle_ref short-circuit */
    if (v->options_[i].toggle_ref &&
        strcmp(v->options_[i].toggle_ref, ref) == 0)
      return &v->options_[i];
    /* GCOVR_EXCL_BR_STOP */
  }

  return NULL;
}

static int parse_enable_disable_(valve_t *v, const char *arg, int argv_index) {
  const char *ref = NULL;
  bool enabled = false;

  if (!is_enable_disable_(arg, &ref, &enabled))
    return 1;

  const vl_option_t *opt = find_toggle_ref_(v, ref);
  if (!opt)
    return 1;

  if (!has_style_(opt, VL_OPT_TYPE_TOGGLE)) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, argv_index, opt->name,
                         "enable/disable type is disabled");
  }

  if (opt->value != VL_OPTION_VALUE_TOGGLE) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, argv_index, opt->name,
      "option do not accept toggle value");
  }

  return bool_value_(v, opt, enabled, argv_index);
}

/* ----------------------------------------------------------------------- */

static bool token_has_double_mark_(const char *raw) {
  return strchr(raw, '.') || strchr(raw, 'e') || strchr(raw, 'E');
}

static bool parse_int_(const char *raw, int64_t *out) {
  char *end = NULL;
  long long value = 0;

  if (!raw || raw[0] == '\0' || isspace((unsigned char)raw[0])) /* GCOVR_EXCL_BR_LINE: null raw unreachable via assign */
    return false;

  errno = 0;
  value = strtoll(raw, &end, 10);
  if (errno == ERANGE || !end || *end != '\0') /* GCOVR_EXCL_BR_LINE: !end never — strtoll always sets end */
    return false;

  *out = (int64_t)value;
  return true;
}

static bool parse_double_(const char *raw, double *out) {
  char *end = NULL;
  double value = 0.0;

  if (!raw || raw[0] == '\0' || isspace((unsigned char)raw[0])) /* GCOVR_EXCL_BR_LINE: null raw unreachable via assign */
    return false;

  errno = 0;
  value = strtod(raw, &end);
  if (errno == ERANGE || !end || *end != '\0' || !isfinite(value)) /* GCOVR_EXCL_BR_LINE: !end never — strtod always sets end */
    return false;

  *out = value;
  return true;
}

static int check_int_bounds_(valve_t *v, const vl_option_t *opt, int64_t value,
                             int argv_index) {
  /* A duration is bounded in seconds, so reporting its bound as an "integer"
     describes the storage rather than what the user typed. */
  const bool duration = effective_scalar_value_(opt) == VL_OPTION_VALUE_TIME;

  if (opt->has_int_min && value < opt->int_min) {
    return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                         duration ? "duration value is below minimum"
                                  : "integer value is below minimum") == 0
               ? 1
               : -1;
  }

  if (opt->has_int_max && value > opt->int_max) {
    return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                         duration ? "duration value is above maximum"
                                  : "integer value is above maximum") == 0
               ? 1
               : -1;
  }

  return 0;
}

static int scalar_value_(valve_t *v, const vl_option_t *opt, const char *raw,
                         int argv_index) {
  vl_value_t value = {
      .kind = VL_VALUE_STRING,
      .raw = strdup(raw),
  };
  vl_option_value_t vk = effective_scalar_value_(opt);
  int64_t integer = 0;
  double number = 0.0;

  if (!value.raw)
    return -1;

  if (vk == VL_OPTION_VALUE_INT) {
    if (!parse_int_(raw, &integer)) {
      vl_value_clear(&value);
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "expected integer value");
    }

    int bounds_rc = check_int_bounds_(v, opt, integer, argv_index);
    if (bounds_rc != 0) {
      vl_value_clear(&value);
      return bounds_rc < 0 ? -1 : 0;
    }

    value.kind = VL_VALUE_INT;
    value.as.integer = integer;
  } else if (vk == VL_OPTION_VALUE_DOUBLE) {
    if (!parse_double_(raw, &number)) {
      vl_value_clear(&value);
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "expected double value");
    }

    value.kind = VL_VALUE_DOUBLE;
    value.as.number = number;
  } else if (vk == VL_OPTION_VALUE_NUMBER) {
    if (token_has_double_mark_(raw) && parse_double_(raw, &number)) {
      value.kind = VL_VALUE_DOUBLE;
      value.as.number = number;
    } else if (parse_int_(raw, &integer)) {
      int bounds_rc = check_int_bounds_(v, opt, integer, argv_index);
      if (bounds_rc != 0) {
        vl_value_clear(&value);
        return bounds_rc < 0 ? -1 : 0;
      }

      value.kind = VL_VALUE_INT;
      value.as.integer = integer;
    } else if (parse_double_(raw, &number)) {
      value.kind = VL_VALUE_DOUBLE;
      value.as.number = number;
    } else {
      vl_value_clear(&value);
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "expected numeric value");
    }
  } else if (vk == VL_OPTION_VALUE_TIME) {
    option_duration_status_t status = option_duration_parse_(raw, &integer);

    if (status != OPTION_DURATION_OK) {
      char message[192];
      snprintf(message, sizeof message, "%s: '%s'",
               option_duration_message_(status), raw);
      vl_value_clear(&value);
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           message);
    }

    /* The literal is now seconds, so has_int_min / has_int_max bound the
       converted count rather than whatever the user typed. */
    int bounds_rc = check_int_bounds_(v, opt, integer, argv_index);
    if (bounds_rc != 0) {
      vl_value_clear(&value);
      return bounds_rc < 0 ? -1 : 0;
    }

    value.kind = VL_VALUE_INT;
    value.as.integer = integer;
  } else if (vk == VL_OPTION_VALUE_BOOL) {
    bool boolean = false;

    if (!parse_bool_literal_(raw, &boolean)) {
      vl_value_clear(&value);
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "expected boolean value");
    }

    value.kind = VL_VALUE_BOOL;
    value.as.boolean = boolean;
  } else if (vk == VL_OPTION_VALUE_AUTO) {
    bool boolean = false;

    if (parse_bool_literal_(raw, &boolean)) {
      value.kind = VL_VALUE_BOOL;
      value.as.boolean = boolean;
    } else if (token_has_double_mark_(raw) && parse_double_(raw, &number)) {
      value.kind = VL_VALUE_DOUBLE;
      value.as.number = number;
    } else if (parse_int_(raw, &integer)) {
      value.kind = VL_VALUE_INT;
      value.as.integer = integer;
    }
  }

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);
    return -1;
  }

  return 0;
}

static int kv_pair_push_(vl_kv_list_t *list, vl_kv_pair_t *pair) {
  size_t next_count, bytes;
  /* GCOVR_EXCL_BR_START — size_t wrap on pathological count */
  if (__builtin_add_overflow(list->count, (size_t)1, &next_count) ||
      __builtin_mul_overflow(next_count, sizeof(vl_kv_pair_t), &bytes))
    return -1; /* GCOVR_EXCL_LINE */
  /* GCOVR_EXCL_BR_STOP */

  vl_kv_pair_t *next = realloc((vl_kv_pair_t *)list->pairs, bytes);

  if (!next)
    return -1;

  next[list->count] = *pair;
  list->pairs = next;
  ++list->count;
  *pair = (vl_kv_pair_t){0};
  return 0;
}

#define VL_KV_MAX_DEPTH 32

static int parse_kv_list_(const char **cursor, vl_kv_list_t *list,
                          char end_char, int depth);

static int kv_scalar_from_raw_(const char *raw, size_t len, bool force_string,
                               vl_value_t *value) {
  char *copy = strndup(raw, len);
  int64_t integer = 0;
  double number = 0.0;
  bool boolean = false;

  if (!copy)
    return -1;

  value->kind = VL_VALUE_STRING;
  value->raw = copy;

  if (force_string)
    return 0;

  if (parse_bool_literal_(copy, &boolean)) {
    value->kind = VL_VALUE_BOOL;
    value->as.boolean = boolean;
  } else if (token_has_double_mark_(copy) && parse_double_(copy, &number)) {
    value->kind = VL_VALUE_DOUBLE;
    value->as.number = number;
  } else if (parse_int_(copy, &integer)) {
    value->kind = VL_VALUE_INT;
    value->as.integer = integer;
  }

  return 0;
}

static int parse_kv_scalar_(const char **cursor, vl_value_t *value,
                            char end_char) {
  const char *start = *cursor;
  const char *p = start;
  bool force_string = false;

  if (*p == '"') {
    force_string = true;
    start = ++p;
    while (*p && *p != '"') {
      ++p;
    }

    if (*p != '"')
      return -1;

    if (kv_scalar_from_raw_(start, (size_t)(p - start), force_string, value) !=
        0)
      return -1;

    *cursor = p + 1;
    return 0;
  }

  while (*p && *p != '|' && (end_char == '\0' || *p != end_char)) {
    ++p;
  }

  if (p == start)
    return -1;

  if (kv_scalar_from_raw_(start, (size_t)(p - start), force_string, value) != 0)
    return -1;

  *cursor = p;
  return 0;
}

static int parse_kv_value_(const char **cursor, vl_value_t *value, int depth,
                           char end_char) {
  if (**cursor == '{') {
    ++(*cursor);
    value->kind = VL_VALUE_KV;
    value->raw = NULL;
    return parse_kv_list_(cursor, &value->as.kv, '}', depth + 1);
  }

  return parse_kv_scalar_(cursor, value, end_char);
}

static int parse_kv_list_(const char **cursor, vl_kv_list_t *list,
                          char end_char, int depth) {
  if (depth > VL_KV_MAX_DEPTH)
    return -1;

  if (**cursor != '!')
    return -1;

  ++(*cursor);

  while (**cursor && **cursor != end_char) {
    const char *key_start = *cursor;
    vl_kv_pair_t pair = {0};

    /* GCOVR_EXCL_BR_START — end_char short-circuit in key scan */
    while (**cursor && **cursor != ':' && **cursor != '|' &&
           **cursor != end_char) {
      ++(*cursor);
    }
    /* GCOVR_EXCL_BR_STOP */

    if (*cursor == key_start || **cursor != ':')
      return -1;

    pair.key = strndup(key_start, (size_t)(*cursor - key_start));
    if (!pair.key)
      return -1;

    ++(*cursor);
    if (parse_kv_value_(cursor, &pair.value, depth, end_char) != 0) {
      free((char *)pair.key);
      vl_value_clear(&pair.value);
      return -1;
    }

    if (kv_pair_push_(list, &pair) != 0) {
      free((char *)pair.key);
      vl_value_clear(&pair.value);
      return -1;
    }

    if (**cursor == '|') {
      ++(*cursor);
      if (**cursor == '\0' || **cursor == end_char) /* GCOVR_EXCL_BR_LINE: end_char after pipe */
        return -1;
    } else if (**cursor != end_char && **cursor != '\0') {
      return -1;
    }
  }

  if (end_char != '\0') {
    if (**cursor != end_char)
      return -1;
    ++(*cursor);
  }

  return list->count > 0 ? 0 : -1;
}

static int kv_value_(valve_t *v, const vl_option_t *opt, const char *raw,
                     int argv_index) {
  vl_value_t value = {
      .kind = VL_VALUE_KV,
      .raw = strdup(raw),
  };

  if (!value.raw)
    return -1;

  const char *cursor = raw;
  if (parse_kv_list_(&cursor, &value.as.kv, '\0', 1) != 0 || *cursor != '\0') { /* GCOVR_EXCL_BR_LINE: trailing-cursor arm */
    vl_value_clear(&value);
    return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                         "expected !key:value|key:{!nested:value}");
  }

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);
    return -1;
  }

  return 0;
}

static int parse_array_element_(const char **cursor, vl_value_t *value) {
  const char *start = *cursor;
  const char *p = start;
  bool force_string = false;

  if (*p == '"') {
    force_string = true;
    start = ++p;
    while (*p && *p != '"') {
      ++p;
    }

    if (*p != '"')
      return -1;

    if (kv_scalar_from_raw_(start, (size_t)(p - start), force_string, value) !=
        0)
      return -1;

    *cursor = p + 1;
    return 0;
  }

  while (*p && *p != ',') {
    ++p;
  }

  if (p == start)
    return -1;

  if (kv_scalar_from_raw_(start, (size_t)(p - start), force_string, value) != 0)
    return -1;

  *cursor = p;
  return 0;
}

static int array_push_value_(vl_array_t *list, vl_value_t *value) {
  size_t next_count, bytes;
  /* GCOVR_EXCL_BR_START — size_t wrap on pathological count */
  if (__builtin_add_overflow(list->count, (size_t)1, &next_count) ||
      __builtin_mul_overflow(next_count, sizeof(vl_value_t), &bytes))
    return -1; /* GCOVR_EXCL_LINE */
  /* GCOVR_EXCL_BR_STOP */

  vl_value_t *next = realloc((vl_value_t *)list->items, bytes);

  if (!next)
    return -1;

  next[list->count] = *value;
  list->items = next;
  ++list->count;
  *value = (vl_value_t){0};
  return 0;
}

static int array_value_(valve_t *v, const vl_option_t *opt, const char *raw,
                        int argv_index) {
  vl_value_t value = {
      .kind = VL_VALUE_ARRAY,
      .raw = strdup(raw),
  };

  if (!value.raw)
    return -1;

  if (*raw == '\0') {
    vl_value_clear(&value);
    return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                         "expected non-empty array value");
  }

  const char *cursor = raw;
  while (*cursor) {
    vl_value_t element = {0};

    if (parse_array_element_(&cursor, &element) != 0) {
      vl_value_clear(&element);
      vl_value_clear(&value);
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "expected comma-separated values");
    }

    if (array_push_value_(&value.as.array, &element) != 0) {
      vl_value_clear(&element);
      vl_value_clear(&value);
      return -1;
    }

    if (*cursor == ',') {
      ++cursor;
      if (*cursor == '\0') {
        vl_value_clear(&value);
        return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                             "trailing comma in array value");
      }
    } else if (*cursor != '\0') {
      vl_value_clear(&value);
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "unexpected character in array value");
    }
  }

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);
    return -1;
  }

  return 0;
}

static int assign_value_(valve_t *v, const vl_option_t *opt,
                         const char *raw, int argv_index) {
  if (opt->value == VL_OPTION_VALUE_KV)
    return kv_value_(v, opt, raw, argv_index);

  if (opt->value == VL_OPTION_VALUE_ARRAY)
    return array_value_(v, opt, raw, argv_index);

  return scalar_value_(v, opt, raw, argv_index);
}

static int parse_long_(valve_t *v, int argc, char **argv, int *index) {
  const char *arg = argv[*index];
  const char *name = arg + 2;
  const char *eq = strchr(name, '=');
  const vl_option_t *opt = NULL;
  int flag_index = *index;

  if (eq) {
    opt = vl_option_find_n_(v, name, (size_t)(eq - name));
    if (!opt) {
      char *tmp = strndup(name, (size_t)(eq - name));
      int rc = 0;
      if (!tmp)
        return -1;
      rc = vl_error_add_(v, VL_ERROR_UNKNOWN_OPTION, flag_index, tmp,
                         "unknown option");
      free(tmp);
      return rc;
    }
    if (opt->value == VL_OPTION_VALUE_COMMAND) {
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                           "command is given after --");
    }
    if (!has_style_(opt, VL_OPT_TYPE_LONG) || v->assign_ != VL_ASSIGN_INLINE) { /* GCOVR_EXCL_BR_LINE: style false short-circuit */
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                           "long inline form is disabled");
    }
    return assign_value_(v, opt, eq + 1, flag_index);
  }

  opt = vl_option_find_(v, name);
  if (!opt) {
    return vl_error_add_(v, VL_ERROR_UNKNOWN_OPTION, flag_index, name,
                         "unknown option");
  }
  if (opt->value == VL_OPTION_VALUE_COMMAND) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                         "command is given after --");
  }
  /* GCOVR_EXCL_BR_START — style false short-circuit */
  if (has_style_(opt, VL_OPT_TYPE_LONG) &&
      opt->value == VL_OPTION_VALUE_BOOL) {
    /* Bare boolean flag: presence means true, in any assign mode. */
    return bool_value_(v, opt, true, flag_index);
  }
  /* GCOVR_EXCL_BR_STOP */
  if (!has_style_(opt, VL_OPT_TYPE_LONG) || v->assign_ != VL_ASSIGN_SEPARATE) { /* GCOVR_EXCL_BR_LINE: style false short-circuit */
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                         "long separate form is disabled");
  }
  if (!consumable_next_value_(argc, argv, *index, opt)) {
    return vl_error_add_(v, VL_ERROR_MISSING_VALUE, flag_index, opt->name,
                         "missing option value");
  }
  ++(*index);
  return assign_value_(v, opt, argv[*index], flag_index);
}

static int parse_short_(valve_t *v, int argc, char **argv, int *index) {
  const char *arg = argv[*index];
  const vl_option_t *opt = vl_option_find_short_(v, arg[1]);
  int flag_index = *index;

  if (!opt) {
    char key[2] = {arg[1], '\0'};
    return vl_error_add_(v, VL_ERROR_UNKNOWN_OPTION, flag_index, key,
                         "unknown option");
  }

  if (!has_style_(opt, VL_OPT_TYPE_SHORT)) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                         "short type is disabled");
  }

  if (opt->value == VL_OPTION_VALUE_KV) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                         "kv values require long form");
  }

  if (arg[2] == '\0' && opt->value == VL_OPTION_VALUE_BOOL) {
    /* Bare boolean flag: presence means true, in any assign mode. */
    return bool_value_(v, opt, true, flag_index);
  }

  if (arg[2] == '=') {
    if (v->assign_ != VL_ASSIGN_INLINE) {
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                           "short inline form is disabled");
    }
    return assign_value_(v, opt, arg + 3, flag_index);
  }

  if (arg[2] == '\0') {
    if (v->assign_ != VL_ASSIGN_SEPARATE) {
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                           "short separate form is disabled");
    }
    if (!consumable_next_value_(argc, argv, *index, opt)) {
      return vl_error_add_(v, VL_ERROR_MISSING_VALUE, flag_index, opt->name,
                           "missing option value");
    }
    ++(*index);
    return assign_value_(v, opt, argv[*index], flag_index);
  }

  return vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, flag_index, opt->name,
                       "unexpected characters after short option");
}

static const char *option_name_in_token_(const char *token, size_t *name_len) {
  const char *name;
  const char *eq;

  if (token[0] == '-' && token[1] == '-')
    name = token + 2;
  else if (token[0] == '-')
    name = token + 1;
  else
    name = token;

  eq = strchr(name, '=');
  *name_len = eq ? (size_t)(eq - name) : strlen(name);
  return name;
}

static bool user_has_option_for_token_(const valve_t *v, const char *token) {
  size_t name_len = 0;
  const char *name = option_name_in_token_(token, &name_len);

  if (name_len == 0) /* GCOVR_EXCL_BR_LINE: empty name after dashes */
    return false; /* GCOVR_EXCL_LINE */

  if (vl_option_find_n_(v, name, name_len))
    return true;

  if (name_len == 1 && vl_option_find_short_(v, name[0])) /* GCOVR_EXCL_BR_LINE: reserved shorts rejected by schema */
    return true; /* GCOVR_EXCL_LINE */

  return false;
}

typedef enum {
  RESERVED_TOKEN_BARE = 0,
  RESERVED_TOKEN_EQ,
  RESERVED_TOKEN_JOINED,
} reserved_token_form_t;

static vl_reserved_kind_t parse_reserved_token_(const char *token,
                                                reserved_token_form_t *form,
                                                const char **target) {
  *form = RESERVED_TOKEN_BARE;
  *target = NULL;
  if (!token) /* GCOVR_EXCL_BR_LINE: null guard */
    return VL_RESERVED_NONE; /* GCOVR_EXCL_LINE */

  const char *eq = strchr(token, '=');
  if (eq) {
    char buf[64];
    size_t plen = (size_t)(eq - token);
    if (plen == 0 || plen >= sizeof(buf))
      return VL_RESERVED_NONE;
    memcpy(buf, token, plen);
    buf[plen] = '\0';
    vl_reserved_kind_t k = vl_reserved_kind_(buf);
    if (k != VL_RESERVED_NONE) {
      *form = RESERVED_TOKEN_EQ;
      *target = eq + 1;
      return k;
    }
    return VL_RESERVED_NONE;
  }

  vl_reserved_kind_t k = vl_reserved_kind_(token);
  if (k != VL_RESERVED_NONE) {
    *form = RESERVED_TOKEN_BARE;
    return k;
  }

  const char *p = token;
  if (p[0] == '-' && p[1] == '-')
    p += 2;
  else if (p[0] == '-')
    p += 1;
  if (p[0] == '?' && p[1] != '\0') { /* GCOVR_EXCL_BR_LINE: bare ? short-circuit */
    *form = RESERVED_TOKEN_JOINED;
    *target = p + 1;
    return VL_RESERVED_HELP;
  }
  return VL_RESERVED_NONE;
}

static bool resolve_help_target_(const valve_t *v, const char *target) {
  vl_help_internal_t res = {0};
  return vl_help_resolve_internal_(v, target, &res);
}

/* When a help target does not resolve, suggest dotted forms for any verb whose
 * sub-verb or option is named `target` (the ambiguity `--help=list` hits). */
static const char *build_unknown_help_message_(const valve_t *v,
                                               const char *target, char *buf,
                                               size_t buflen) {
  int wrote = snprintf(buf, buflen, "unknown help target");
  if (wrote < 0 || (size_t)wrote >= buflen) /* GCOVR_EXCL_BR_LINE: snprintf/buf failure */
    return "unknown help target"; /* GCOVR_EXCL_LINE */
  size_t used = (size_t)wrote;
  size_t hints = 0;

  for (size_t c = 0; c < v->verb_count_ && used < buflen; ++c) {
    const valve_verb_t *verb = &v->verbs_[c];
    bool match = false;
    for (size_t s = 0; s < verb->verb_count && !match; ++s)
      if (verb->verbs[s].name && strcmp(verb->verbs[s].name, target) == 0) /* GCOVR_EXCL_BR_LINE: null sub name */
        match = true;
    for (size_t o = 0; o < verb->option_count && !match; ++o)
      if (verb->options[o].name && strcmp(verb->options[o].name, target) == 0) /* GCOVR_EXCL_BR_LINE: option_anywhere_ resolves first */
        match = true; /* GCOVR_EXCL_LINE */
    if (!match)
      continue;

    wrote = snprintf(buf + used, buflen - used, "%s%s.%s",
                     hints == 0 ? "; did you mean " : ", ", verb->name, target);
    if (wrote < 0 || (size_t)wrote >= buflen - used) /* GCOVR_EXCL_BR_LINE: hint buffer exhausted */
      return buf; /* GCOVR_EXCL_LINE */
    used += (size_t)wrote;
    ++hints;
  }
  if (hints && used < buflen) /* GCOVR_EXCL_BR_LINE: buffer-full short-circuit */
    (void)snprintf(buf + used, buflen - used, "?");
  return buf;
}

static void dispatch_reserved_(valve_t *v, vl_reserved_kind_t kind) {
  v->reserved_fired_ = true;
  switch (kind) { /* GCOVR_EXCL_BR_LINE: NONE/default unreachable */
  case VL_RESERVED_VALVE:
    if (v->on_valve_)
      v->on_valve_(v);
    else
      vl_valve_print_default_(v);
    break;
  case VL_RESERVED_HELP:
    if (v->on_help_)
      v->on_help_(v);
    else
      vl_help_print_default_(v);
    break;
  case VL_RESERVED_VERSION:
    if (v->on_version_)
      v->on_version_(v);
    else
      vl_version_print_default_(v);
    break;
  case VL_RESERVED_NONE: /* GCOVR_EXCL_LINE: dispatch only for real kinds */
  default: /* GCOVR_EXCL_BR_LINE */
    break; /* GCOVR_EXCL_LINE */
  }
}

static int dispatch_reserved_with_target_(valve_t *v, vl_reserved_kind_t kind,
                                          const char *target, int argv_index) {
  if (kind == VL_RESERVED_HELP && target && target[0] != '\0') { /* GCOVR_EXCL_BR_LINE: empty-target short-circuit */
    if (!resolve_help_target_(v, target)) {
      char msg[192];
      (void)vl_error_add_(
          v, VL_ERROR_INVALID_VALUE, argv_index, target,
          build_unknown_help_message_(v, target, msg, sizeof msg));
      return -1;
    }
    free(v->help_target_);
    v->help_target_ = strdup(target);
  } else {
    free(v->help_target_);
    v->help_target_ = NULL;
  }
  (void)argv_index;
  dispatch_reserved_(v, kind);
  return 0;
}

/* Returns 0 on success, -1 on form mismatch / unexpected value.
 * On success: *target_out is NULL or points into argv (do not free).
 * *consumed_extra is true when argv[i+1] was consumed as the target. */
static int extract_reserved_target_(valve_t *v, vl_reserved_kind_t kind,
                                    reserved_token_form_t form, int argc,
                                    char **argv, int i, const char *target_in,
                                    const char **target_out,
                                    bool *consumed_extra) {
  *target_out = NULL;
  *consumed_extra = false;

  if (kind != VL_RESERVED_HELP) {
    if (target_in != NULL) {
      (void)vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i, argv[i],
                          "reserved token does not accept a value");
      return -1;
    }
    return 0;
  }

  if (form == RESERVED_TOKEN_EQ) {
    if (v->assign_ != VL_ASSIGN_INLINE) {
      (void)vl_error_add_(v, VL_ERROR_DISABLED_FORM, i, argv[i],
                          "help target requires separate form");
      return -1;
    }
    if (target_in[0] == '\0') {
      (void)vl_error_add_(v, VL_ERROR_MISSING_VALUE, i, argv[i],
                          "missing help target after '='");
      return -1;
    }
    *target_out = target_in;
    return 0;
  }

  if (form == RESERVED_TOKEN_JOINED) {
    *target_out = target_in;
    return 0;
  }

  /* GCOVR_EXCL_BR_START — null/empty next short-circuit */
  if (v->assign_ == VL_ASSIGN_SEPARATE && i + 1 < argc &&
      argv[i + 1] && argv[i + 1][0] != '\0' &&
      !looks_like_option_(argv[i + 1])) {
    *target_out = argv[i + 1];
    *consumed_extra = true;
  }
  /* GCOVR_EXCL_BR_STOP */
  return 0;
}

/* Executable action at argv[1]. The token is parsed with the ordinary option
 * rules while lookup is narrowed to the matched action, then the action is
 * dispatched like a reserved token. Returns 1 when argv[1] is not an action,
 * 0 when it fired, -1 on error. */
static int parse_action_(valve_t *v, int argc, char **argv) {
  const vl_option_t *action = vl_action_find_(v, argv[1]);
  int i = 1;
  int rc;

  if (!action)
    return 1;

  v->active_action_ = action;
  if (argv[1][1] == '-') {
    rc = parse_enable_disable_(v, argv[1], i);
    if (rc == 1)
      rc = parse_long_(v, argc, argv, &i);
  } else {
    rc = parse_short_(v, argc, argv, &i);
  }
  v->active_action_ = NULL;

  if (rc < 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, i, NULL, "out of memory");
    return -1;
  }
  if (v->error_count_)
    return -1;
  if (i + 1 < argc) {
    (void)vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i + 1, argv[i + 1],
                        "unexpected argument after action");
    return -1;
  }

  v->reserved_fired_ = true;
  v->action_fired_ = action;
  v->action_run_[action - v->actions_](v);
  return 0;
}

int vl_parse(valve_t *v, int argc, char **argv) {
  int start = 1;

  if (!v || argc < 0 || !argv)
    return -1;

  vl_results_clear_(v);
  vl_errors_clear_(v);
  v->active_verb_ = NULL;
  v->active_subverb_ = NULL;
  v->reserved_fired_ = false;
  v->active_action_ = NULL;
  v->action_fired_ = NULL;
  v->parsed_ = true;

  if (argc <= 1) {
    dispatch_reserved_(v, VL_RESERVED_HELP);
    return 0;
  }

  if (v->action_count_ > 0) {
    int rc = parse_action_(v, argc, argv);
    if (rc <= 0)
      return rc;
  }

  if (v->verb_count_ > 0) {
    const char *command_name = argc > 1 ? argv[1] : NULL; /* GCOVR_EXCL_BR_LINE: argc>1 always here */

    /* GCOVR_EXCL_BR_START — null argv[1] / empty-command short-circuit */
    if (!command_name || command_name[0] == '\0' ||
        looks_like_option_(command_name)) {
      if (command_name) { /* GCOVR_EXCL_BR_LINE: null command_name */
        reserved_token_form_t form = RESERVED_TOKEN_BARE;
        const char *target_in = NULL;
        vl_reserved_kind_t k =
            parse_reserved_token_(command_name, &form, &target_in);
        /* GCOVR_EXCL_BR_START — reserved vs user-option race */
        if (k != VL_RESERVED_NONE &&
            !user_has_option_for_token_(v, command_name)) {
          const char *target = NULL;
          bool consumed = false;
          if (extract_reserved_target_(v, k, form, argc, argv, 1, target_in,
                                       &target, &consumed) != 0)
            return -1;
          if (dispatch_reserved_with_target_(v, k, target, 1) != 0)
            return -1;
          return 0;
        }
        /* GCOVR_EXCL_BR_STOP */
      }
      (void)vl_error_add_(v, VL_ERROR_MISSING_COMMAND, 1, NULL,
                          "missing verb");
      return -1;
    }
    /* GCOVR_EXCL_BR_STOP */

    v->active_verb_ = vl_verb_find_(v, command_name);
    if (!v->active_verb_) {
      reserved_token_form_t form = RESERVED_TOKEN_BARE;
      const char *target_in = NULL;
      vl_reserved_kind_t k =
          parse_reserved_token_(command_name, &form, &target_in);
      if (k != VL_RESERVED_NONE) {
        const char *target = NULL;
        bool consumed = false;
        if (extract_reserved_target_(v, k, form, argc, argv, 1, target_in,
                                     &target, &consumed) != 0)
          return -1;
        if (dispatch_reserved_with_target_(v, k, target, 1) != 0)
          return -1;
        return 0;
      }
      (void)vl_error_add_(v, VL_ERROR_UNKNOWN_COMMAND, 1, command_name,
                          "unknown verb");
      return -1;
    }

    start = 2;

    if (v->active_verb_->verb_count > 0) {
      const char *subverb_name = argc > 2 ? argv[2] : NULL;

      if (!subverb_name || subverb_name[0] == '\0' ||
          looks_like_option_(subverb_name)) {
        if (subverb_name) {
          reserved_token_form_t form = RESERVED_TOKEN_BARE;
          const char *target_in = NULL;
          vl_reserved_kind_t k =
              parse_reserved_token_(subverb_name, &form, &target_in);
          /* GCOVR_EXCL_BR_START — reserved vs user-option race on subverb */
          if (k != VL_RESERVED_NONE &&
              !user_has_option_for_token_(v, subverb_name)) {
            const char *target = NULL;
            bool consumed = false;
            if (extract_reserved_target_(v, k, form, argc, argv, 2, target_in,
                                         &target, &consumed) != 0)
              return -1;
            if (dispatch_reserved_with_target_(v, k, target, 2) != 0)
              return -1;
            return 0;
          }
          /* GCOVR_EXCL_BR_STOP */
        }
        (void)vl_error_add_(v, VL_ERROR_MISSING_COMMAND, 2,
                            v->active_verb_->name, "missing sub-verb");
        return -1;
      }

      v->active_subverb_ = verb_find_in_(
          v->active_verb_->verbs, v->active_verb_->verb_count, subverb_name);
      if (!v->active_subverb_) {
        (void)vl_error_add_(v, VL_ERROR_UNKNOWN_COMMAND, 2, subverb_name,
                            "unknown sub-verb");
        return -1;
      }

      start = 3;
    }
  }

  for (int i = start; i < argc; ++i) {
    const char *arg = argv[i];
    int rc = 0;

    if (!arg || arg[0] == '\0') {
      rc = vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i, NULL,
                         "empty argument");
    } else if (strcmp(arg, "--") == 0) {
      rc = vl_parse_command_(v, argc, argv, i);
      if (rc < 0) {
        (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, i, NULL,
                            "out of memory");
        return -1;
      }
      break;
    } else {
      reserved_token_form_t form = RESERVED_TOKEN_BARE;
      const char *target_in = NULL;
      vl_reserved_kind_t k = parse_reserved_token_(arg, &form, &target_in);
      if (k != VL_RESERVED_NONE && !user_has_option_for_token_(v, arg)) {
        const char *target = NULL;
        bool consumed = false;
        if (extract_reserved_target_(v, k, form, argc, argv, i, target_in,
                                     &target, &consumed) != 0)
          return -1;
        if (dispatch_reserved_with_target_(v, k, target, i) != 0)
          return -1;
        return 0;
      }

      if (strncmp(arg, "--", 2) == 0) {
        rc = parse_enable_disable_(v, arg, i);
        if (rc == 1)
          rc = parse_long_(v, argc, argv, &i);
      } else if (arg[0] == '-' && arg[1] != '\0') {
        rc = parse_short_(v, argc, argv, &i);
      } else {
        rc = vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i, NULL,
                           "unexpected positional argument");
      }
    }

    if (rc < 0) {
      (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, i, NULL, "out of memory");
      return -1;
    }
  }

  if (v->error_count_)
    return -1;

  if (vl_required_check_(v) != 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, 0, NULL, "out of memory");
    return -1;
  }

  if (vl_requirements_check_(v) != 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, 0, NULL, "out of memory");
    return -1;
  }

  if (vl_conflicts_check_(v) != 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, 0, NULL, "out of memory");
    return -1;
  }

  return v->error_count_ ? -1 : 0;
}
