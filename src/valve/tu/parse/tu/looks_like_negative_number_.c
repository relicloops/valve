#include "../private.h"

bool vl_parse_looks_like_negative_number_(const char *s) {
  if (!s || s[0] != '-') /* GCOVR_EXCL_BR_LINE: callers pass non-null option-like tokens */
    return false; /* GCOVR_EXCL_LINE */
  if (s[1] >= '0' && s[1] <= '9')
    return true;
  return s[1] == '.' && s[2] >= '0' && s[2] <= '9';
}
