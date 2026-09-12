#include "valve/valve.h"

const char *vl_version_string(void) {
  /* 🜖  = UTF-8 0xf0 0x9f 0x9c 0x96 */
  return "\xf0\x9f\x9c\x96 valve v" VALVE_VERSION;
}
