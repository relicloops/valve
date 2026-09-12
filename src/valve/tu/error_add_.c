#include "../private.h"

#include <stdlib.h>
#include <string.h>

int vl_error_add_(valve_t *v, vl_error_code_t code, int argv_index,
                  const char *key, const char *message) {
  if (!v)
    return -1;

  if (v->error_count_ == v->error_cap_) {
    size_t next_cap;
    if (v->error_cap_) {
      if (__builtin_mul_overflow(v->error_cap_, (size_t)2, &next_cap))
        return -1;
    } else {
      next_cap = 4;
    }
    size_t bytes;
    if (__builtin_mul_overflow(next_cap, sizeof(vl_error_t), &bytes))
      return -1;
    vl_error_t *next = realloc(v->errors_, bytes);
    if (!next)
      return -1;
    v->errors_ = next;
    v->error_cap_ = next_cap;
  }

  vl_error_t *err = &v->errors_[v->error_count_];
  *err = (vl_error_t){
      .argv_index = argv_index,
      .code = code,
  };

  if (key) {
    err->key = strdup(key);
    if (!err->key)
      return -1;
  }

  if (message) {
    err->message = strdup(message);
    if (!err->message) {
      free((char *)err->key);
      err->key = NULL;
      return -1;
    }
  }

  ++v->error_count_;
  return 0;
}
