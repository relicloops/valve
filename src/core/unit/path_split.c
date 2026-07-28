#include "../valve_private.h"

#include <string.h>

size_t vl_path_split(const char *dotted, char *scratch, size_t scratch_len,
                     vl_path_t *out) {
  if (!dotted || !scratch || !out || scratch_len == 0)
    return 0;

  *out = (vl_path_t){0};

  size_t len = strlen(dotted);
  if (len == 0 || len >= scratch_len)
    return 0;

  memcpy(scratch, dotted, len + 1);

  size_t count = 0;
  char *seg = scratch;

  for (char *p = scratch;; ++p) {
    if (*p != '.' && *p != '\0')
      continue;

    if (p == seg || count >= VL_PATH_MAX_SEGMENTS) {
      *out = (vl_path_t){0};
      return 0;
    }

    char terminal = *p;
    *p = '\0';
    out->segments[count++] = seg;

    if (terminal == '\0')
      break;
    seg = p + 1;
  }

  out->count = count;
  return count;
}
