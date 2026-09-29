#include "../../private.h"

bool vl_has(const valve_t *v, const char *key) {

  return vl_get(v, key) != nullptr;
}
