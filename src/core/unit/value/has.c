#include "../../valve_private.h"

#include <stdbool.h>

bool vl_has(const valve_t *v, const char *key) {
  return vl_get(v, key) != NULL;
}
