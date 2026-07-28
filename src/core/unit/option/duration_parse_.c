#include "../../valve_private.h"

/* Duration literal -> whole seconds.
 *
 * Grammar:
 *
 *   duration := digits | segment+
 *   segment  := digits unit
 *   unit     := 's' | 'm' | 'h' | 'd'
 *
 * A literal with no unit is seconds, so an option migrating here from
 * VL_OPTION_VALUE_INT keeps accepting everything it accepted before. A
 * suffix-less run is therefore only legal as the entire literal: "1h30" is
 * rejected rather than guessed at, because both "1h30s" and "1h30m" are
 * plausible readings of it.
 *
 * Segments are summed, which is what makes "1h30m" work. No ordering or
 * uniqueness is imposed on the units.
 */

static int64_t unit_seconds_(char c) {
  switch (c) {
  case 's':
    return 1;
  case 'm':
    return 60;
  case 'h':
    return 3600;
  case 'd':
    return 86400;
  default:
    return 0;
  }
}

option_duration_status_t option_duration_parse_(const char *raw, int64_t *out) {
  if (!raw || raw[0] == '\0')
    return OPTION_DURATION_EMPTY;

  /* Reported separately from a syntax error: a negative duration is a
     meaningful thing to ask for and a meaningless thing to have. */
  if (raw[0] == '-')
    return OPTION_DURATION_NEGATIVE;

  int64_t total = 0;
  const char *p = raw;
  bool any_segment = false;

  while (*p != '\0') {
    if (*p < '0' || *p > '9')
      return OPTION_DURATION_SYNTAX;

    int64_t digits = 0;
    while (*p >= '0' && *p <= '9') {
      if (__builtin_mul_overflow(digits, (int64_t)10, &digits))
        return OPTION_DURATION_OVERFLOW;
      if (__builtin_add_overflow(digits, (int64_t)(*p - '0'), &digits))
        return OPTION_DURATION_OVERFLOW;
      ++p;
    }

    if (*p == '\0') {
      /* Bare integer: seconds, but only when it is the whole literal. */
      if (any_segment)
        return OPTION_DURATION_SYNTAX;
      if (__builtin_add_overflow(total, digits, &total)) /* GCOVR_EXCL_BR_LINE: total is 0 here */
        return OPTION_DURATION_OVERFLOW; /* GCOVR_EXCL_LINE */
      *out = total;
      return OPTION_DURATION_OK;
    }

    int64_t scale = unit_seconds_(*p);
    if (scale == 0)
      return OPTION_DURATION_SUFFIX;
    ++p;

    int64_t seconds = 0;
    if (__builtin_mul_overflow(digits, scale, &seconds))
      return OPTION_DURATION_OVERFLOW;
    if (__builtin_add_overflow(total, seconds, &total))
      return OPTION_DURATION_OVERFLOW;

    any_segment = true;
  }

  if (!any_segment) /* GCOVR_EXCL_BR_LINE: empty caught at entry */
    return OPTION_DURATION_EMPTY; /* GCOVR_EXCL_LINE */

  *out = total;
  return OPTION_DURATION_OK;
}

const char *option_duration_message_(option_duration_status_t status) {
  switch (status) {
  case OPTION_DURATION_EMPTY:
    return "expected duration value, got empty value";
  case OPTION_DURATION_NEGATIVE:
    return "duration value cannot be negative";
  case OPTION_DURATION_SUFFIX:
    return "duration value has an unknown unit, expected s, m, h, or d";
  case OPTION_DURATION_OVERFLOW:
    return "duration value is too large";
  case OPTION_DURATION_SYNTAX:
  case OPTION_DURATION_OK:
  default:
    return "expected duration value such as 30, 45s, 5m, 2h, or 1h30m";
  }
}
