#include "../private.h"

#include <string.h>

const char *vl_parse_option_name_in_token_(const char *token,
                                           size_t *name_len) {
  const char *name = vl_reserved_strip_dashes_(token);
  const char *eq = strchr(name, '=');

  *name_len = eq ? (size_t)(eq - name) : strlen(name);
  return name;
}
