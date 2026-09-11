#include "../private.h"

#include <string.h>

static const char *strip_dashes_(const char *token) {
  if (!token)
    return NULL;
  if (token[0] == '-' && token[1] == '-')
    return token + 2;
  if (token[0] == '-')
    return token + 1;
  return token;
}

vl_reserved_kind_t vl_reserved_name_(const char *name) {
  if (!name || name[0] == '\0')
    return VL_RESERVED_NONE;

  if (strcmp(name, "valve") == 0)
    return VL_RESERVED_VALVE;
  if (strcmp(name, "help") == 0 || strcmp(name, "?") == 0 ||
      strcmp(name, "h") == 0)
    return VL_RESERVED_HELP;
  if (strcmp(name, "version") == 0 || strcmp(name, "v") == 0)
    return VL_RESERVED_VERSION;

  return VL_RESERVED_NONE;
}

vl_reserved_kind_t vl_reserved_short_(char c) {
  if (c == 'h' || c == '?')
    return VL_RESERVED_HELP;
  if (c == 'v')
    return VL_RESERVED_VERSION;
  return VL_RESERVED_NONE;
}

vl_reserved_kind_t vl_reserved_kind_(const char *token) {
  const char *name = strip_dashes_(token);
  if (!name || name[0] == '\0')
    return VL_RESERVED_NONE;
  return vl_reserved_name_(name);
}
