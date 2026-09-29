#include "../private.h"

#include <string.h>

bool vl_parse_token_has_double_mark_(const char *raw) {

  return strchr(raw, '.') || strchr(raw, 'e') || strchr(raw, 'E');
}
