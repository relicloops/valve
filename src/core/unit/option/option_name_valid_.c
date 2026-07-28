#include "../../valve_private.h"

#include <string.h>

bool option_name_valid_(const char *name) {
  return name && name[0] != '\0' && name[0] != '-' && !strchr(name, '=');
}