#include "../private.h"

bool vl_parse_looks_like_option_(const char *s) {

  return s && s[0] == '-' && s[1] != '\0'; /* GCOVR_EXCL_BR_LINE: null only via pathological argv */
}
