#include "../../private.h"

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef enum scalar_shape {
  SHAPE_TEXT = 0,
  SHAPE_INT,
  SHAPE_DOUBLE,
} scalar_shape_t;

static bool digit_(char c) { return c >= '0' && c <= '9'; }

/* Strict decimal grammar, scanned by hand:
 *   [+-]? ( digits ( '.' digits* )? | '.' digits ) ( [eE] [+-]? digits )?
 * Hexadecimal, inf and nan are deliberately text; strtod would accept them
 * and quietly turn identifiers such as 0x1e3f into numbers. */
static scalar_shape_t decimal_shape_(const char *s) {
  const char *p = s;
  bool digits = false;
  bool fraction = false;
  bool exponent = false;

  if (*p == '+' || *p == '-')
    ++p;
  while (digit_(*p)) {
    ++p;
    digits = true;
  }
  if (*p == '.') {
    bool fraction_digits = false;

    ++p;
    fraction = true;
    while (digit_(*p)) {
      ++p;
      fraction_digits = true;
    }
    if (!digits && !fraction_digits)
      return SHAPE_TEXT;
  } else if (!digits) {
    return SHAPE_TEXT;
  }
  if (*p == 'e' || *p == 'E') {
    ++p;
    exponent = true;
    if (*p == '+' || *p == '-')
      ++p;
    if (!digit_(*p))
      return SHAPE_TEXT;
    while (digit_(*p))
      ++p;
  }
  if (*p != '\0')
    return SHAPE_TEXT;
  return (fraction || exponent) ? SHAPE_DOUBLE : SHAPE_INT;
}

option_scalar_status_t option_scalar_auto_(vl_value_t *value) {
  const char *raw = value->raw;

  value->kind = VL_VALUE_STRING;

  if (strcmp(raw, "true") == 0 || strcmp(raw, "false") == 0) {
    value->kind = VL_VALUE_BOOL;
    value->as.boolean = raw[0] == 't';
    return OPTION_SCALAR_OK;
  }

  switch (decimal_shape_(raw)) { /* GCOVR_EXCL_BR_LINE: default arm unreachable, decimal_shape_ returns only the three shapes */
  case SHAPE_INT: {
    errno = 0;
    long long integer = strtoll(raw, nullptr, 10);

    if (errno == ERANGE)
      return OPTION_SCALAR_INT_RANGE;
    value->kind = VL_VALUE_INT;
    value->as.integer = (int64_t)integer;
    return OPTION_SCALAR_OK;
  }
  case SHAPE_DOUBLE: {
    errno = 0;
    double number = option_strtod_c_(raw, nullptr);

    if (errno == ERANGE || !isfinite(number)) /* GCOVR_EXCL_BR_LINE: isfinite false only for inf/nan, excluded by the grammar */
      return OPTION_SCALAR_DOUBLE_RANGE;
    value->kind = VL_VALUE_DOUBLE;
    value->as.number = number;
    return OPTION_SCALAR_OK;
  }
  case SHAPE_TEXT:
  default:
    return OPTION_SCALAR_OK;
  }
}

const char *option_scalar_message_(option_scalar_status_t status) {
  switch (status) {
  case OPTION_SCALAR_INT_RANGE:
    return "integer value does not fit 64 bits";
  case OPTION_SCALAR_DOUBLE_RANGE:
    return "decimal value is out of range";
  case OPTION_SCALAR_OK:
  default:
    return "value is valid";
  }
}
