#include "../private.h"

#include <string.h>

/* Returns 0 on success, -1 on a syntax error (message left NULL), -1 with
 * *message set on a range error, -2 on allocation failure. */
int vl_parse_array_element_(const char **cursor, vl_value_t *value,
                            const char **message) {
  const char *start = *cursor;
  const char *p = start;

  *message = nullptr;

  if (*p == '"') {
    start = ++p;
    while (*p && *p != '"') {
      ++p;
    }

    if (*p != '"')
      return -1;

    value->kind = VL_VALUE_STRING;
    value->raw = strndup(start, (size_t)(p - start));
    if (!value->raw)
      return -2;

    *cursor = p + 1;
    return 0;
  }

  while (*p && *p != ',') {
    ++p;
  }

  if (p == start)
    return -1;

  value->raw = strndup(start, (size_t)(p - start));
  if (!value->raw)
    return -2;

  option_scalar_status_t status = option_scalar_auto_(value);
  if (status != OPTION_SCALAR_OK) {
    *message = option_scalar_message_(status);
    return -1;
  }

  *cursor = p;
  return 0;
}
