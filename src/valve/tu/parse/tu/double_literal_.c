#include "../private.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>

bool vl_parse_double_literal_(const char *raw, double *out) {
  char *end = nullptr;
  double value = 0.0;

  if (!raw || raw[0] == '\0' || isspace((unsigned char)raw[0])) /* GCOVR_EXCL_BR_LINE: null raw unreachable via assign */
    return false;

  errno = 0;
  value = option_strtod_c_(raw, &end);
  if (errno == ERANGE || !end || *end != '\0' || !isfinite(value)) /* GCOVR_EXCL_BR_LINE: !end never — strtod always sets end */
    return false;

  *out = value;
  return true;
}
