#include "../../valve_private.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

static int value_clone_(vl_value_t *dst, const vl_value_t *src) {
  *dst = (vl_value_t){.kind = src->kind};

  if (src->raw) {
    dst->raw = strdup(src->raw);
    if (!dst->raw)
      goto fail;
  }

  if (src->kind == VL_VALUE_INT) {
    dst->as.integer = src->as.integer;
  } else if (src->kind == VL_VALUE_DOUBLE) {
    dst->as.number = src->as.number;
  } else if (src->kind == VL_VALUE_BOOL) {
    dst->as.boolean = src->as.boolean;
  } else if (src->kind == VL_VALUE_COMMAND) {
    dst->as.command = src->as.command;
  } else if (src->kind == VL_VALUE_KV) {
    vl_kv_pair_t *pairs = NULL;

    if (src->as.kv.count > 0) {
      pairs = calloc(src->as.kv.count, sizeof(vl_kv_pair_t));
      if (!pairs)
        goto fail;
    }

    dst->as.kv.pairs = pairs;
    dst->as.kv.count = src->as.kv.count;

    for (size_t i = 0; i < src->as.kv.count; ++i) {
      pairs[i].key = strdup(src->as.kv.pairs[i].key);
      if (!pairs[i].key)
        goto fail;
      if (value_clone_(&pairs[i].value, &src->as.kv.pairs[i].value) != 0)
        goto fail;
    }
  } else if (src->kind == VL_VALUE_ARRAY) {
    vl_value_t *items = NULL;

    if (src->as.array.count > 0) {
      items = calloc(src->as.array.count, sizeof(vl_value_t));
      if (!items)
        goto fail;
    }

    dst->as.array.items = items;
    dst->as.array.count = src->as.array.count;

    for (size_t i = 0; i < src->as.array.count; ++i) {
      if (value_clone_(&items[i], &src->as.array.items[i]) != 0)
        goto fail;
    }
  }

  return 0;

fail:
  vl_value_clear(dst);
  return -1;
}

static void *target_ptr_(const vl_option_t *opt) {
  if (!opt->data || opt->target == VL_TARGET_NONE)
    return NULL;

  return (char *)opt->data + opt->offset;
}

static int target_set_(valve_t *v, const vl_option_t *opt,
                       const vl_value_t *value, int argv_index) {
  void *target = target_ptr_(opt);

  if (!target)
    return 0;

  switch (opt->target) { /* GCOVR_EXCL_BR_LINE: NONE unreachable — target_ptr_ returns NULL first */
  case VL_TARGET_NONE: /* GCOVR_EXCL_BR_LINE: target_ptr_ returns NULL for NONE first */
    return 0; /* GCOVR_EXCL_LINE */
  case VL_TARGET_STRING: {
    if (value->kind != VL_VALUE_STRING)
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "target expects string value");
    char *copy = strdup(value->raw);
    if (!copy)
      return -1;
    free(*(char **)target);
    *(char **)target = copy;
    return 0;
  }
  case VL_TARGET_INT:
    if (value->kind != VL_VALUE_INT)
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "target expects int value");
    if (value->as.integer < INT_MIN || value->as.integer > INT_MAX)
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "integer value does not fit target int");
    *(int *)target = (int)value->as.integer;
    return 0;
  case VL_TARGET_INT64:
    if (value->kind != VL_VALUE_INT)
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "target expects int64 value");
    *(int64_t *)target = value->as.integer;
    return 0;
  case VL_TARGET_DOUBLE:
    if (value->kind != VL_VALUE_DOUBLE)
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "target expects double value");
    *(double *)target = value->as.number;
    return 0;
  case VL_TARGET_BOOL:
    if (value->kind != VL_VALUE_BOOL)
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "target expects bool value");
    *(bool *)target = value->as.boolean;
    return 0;
  case VL_TARGET_COMMAND:
    if (value->kind != VL_VALUE_COMMAND)
      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name,
                           "target expects command value");
    *(vl_command_t *)target = value->as.command;
    return 0;
  case VL_TARGET_TOGGLE:
  case VL_TARGET_VALUE: {
    vl_value_t next = {0};
    if (value_clone_(&next, value) != 0)
      return -1;
    vl_value_clear((vl_value_t *)target);
    *(vl_value_t *)target = next;
    return 0;
  }
  }

  return 0;
}

static int append_array_(vl_result_t *result, vl_value_t *value,
                         int argv_index) {
  vl_value_t *items = (vl_value_t *)result->value.as.array.items;
  size_t count = result->value.as.array.count;
  size_t next_count, bytes;
  /* GCOVR_EXCL_BR_START — size_t wrap */
  if (__builtin_add_overflow(count, (size_t)1, &next_count) ||
      __builtin_mul_overflow(next_count, sizeof(vl_value_t), &bytes))
    return -1; /* GCOVR_EXCL_LINE */
  /* GCOVR_EXCL_BR_STOP */

  vl_value_t *next = realloc(items, bytes);

  if (!next)
    return -1;

  next[count] = *value;
  result->value.as.array.items = next;
  result->value.as.array.count = count + 1;
  result->argv_index = argv_index;
  *value = (vl_value_t){0};
  return 0;
}

static int convert_to_array_(vl_result_t *result, vl_value_t *value,
                             int argv_index) {
  vl_value_t old = result->value;
  vl_value_t *items = calloc(2, sizeof(vl_value_t));

  if (!items)
    return -1;

  items[0] = old;
  items[1] = *value;
  result->value = (vl_value_t){
      .kind = VL_VALUE_ARRAY,
      .as.array =
          {
              .items = items,
              .count = 2,
          },
  };
  result->argv_index = argv_index;
  *value = (vl_value_t){0};
  return 0;
}

int vl_result_set_(valve_t *v, const vl_option_t *opt, vl_value_t *value,
                   int argv_index) {
  if (!v || !opt || !opt->name || !value)
    return -1;

  if (v->results_) {
    for (size_t i = 0; i < v->result_count_; ++i) {
      if (strcmp(v->results_[i].key, opt->name) == 0) {
        if (opt->repeat == VL_OPTION_REPEAT_ERROR) {
          int rc = vl_error_add_(v, VL_ERROR_DUPLICATE_OPTION, argv_index,
                                 opt->name, "duplicate option");
          vl_value_clear(value);
          return rc;
        }

        bool is_wrapped_list = v->results_[i].value.kind == VL_VALUE_ARRAY &&
                               v->results_[i].value.raw == NULL;
        if (is_wrapped_list)
          return append_array_(&v->results_[i], value, argv_index) == 0
                     ? target_set_(v, opt, &v->results_[i].value, argv_index)
                     : -1;

        if (convert_to_array_(&v->results_[i], value, argv_index) != 0)
          return -1;
        return target_set_(v, opt, &v->results_[i].value, argv_index);
      }
    }
  }

  if (v->result_count_ == v->result_cap_) {
    size_t next_cap;
    if (v->result_cap_) {
      if (__builtin_mul_overflow(v->result_cap_, (size_t)2, &next_cap))
        return -1;
    } else {
      next_cap = 8;
    }
    size_t bytes;
    if (__builtin_mul_overflow(next_cap, sizeof(vl_result_t), &bytes))
      return -1;
    vl_result_t *next = realloc(v->results_, bytes);
    if (!next)
      return -1;
    v->results_ = next;
    v->result_cap_ = next_cap;
  }

  vl_result_t *result = &v->results_[v->result_count_];
  *result = (vl_result_t){
      .key = strdup(opt->name),
      .value = *value,
      .argv_index = argv_index,
  };

  if (!result->key) {
    *result = (vl_result_t){0};
    return -1;
  }

  *value = (vl_value_t){0};
  ++v->result_count_;
  if (target_set_(v, opt, &result->value, argv_index) != 0)
    return -1;
  return 0;
}
