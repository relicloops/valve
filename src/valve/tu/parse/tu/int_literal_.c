#include "../private.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

bool vl_parse_int_literal_(const char *raw, int64_t *out) {
  char *end = nullptr;
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
