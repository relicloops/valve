#include "../private.h"

#include <string.h>

bool vl_parse_is_enable_disable_(const char *arg, const char **name, bool *enabled) {

  const char *enable = "--enable-";
  const char *disable = "--disable-";
  size_t enable_len = strlen(enable);
  size_t disable_len = strlen(disable);

  if (strncmp(arg, enable, enable_len) == 0 && arg[enable_len] != '\0') {
    *name = arg + enable_len;
    *enabled = true;

    return true;
  }

  if (strncmp(arg, disable, disable_len) == 0 && arg[disable_len] != '\0') {
    *name = arg + disable_len;
    *enabled = false;

    return true;
  }

  return false;
}
