#include "../private.h"

#include <string.h>

vl_reserved_kind_t vl_parse_reserved_token_(const char *token,
                                            vl_parse_reserved_form_t *form,
                                            const char **target) {
  *form = VL_PARSE_RESERVED_BARE;
  *target = nullptr;
  if (!token) /* GCOVR_EXCL_BR_LINE: null guard */
    return VL_RESERVED_NONE; /* GCOVR_EXCL_LINE */

  const char *eq = strchr(token, '=');
  if (eq) {
    char buf[64];
    size_t plen = (size_t)(eq - token);
    if (plen == 0 || plen >= sizeof(buf))
      return VL_RESERVED_NONE;
    memcpy(buf, token, plen);
    buf[plen] = '\0';
    vl_reserved_kind_t k = vl_reserved_kind_(buf);
    if (k != VL_RESERVED_NONE) {
      *form = VL_PARSE_RESERVED_EQ;
      *target = eq + 1;
      return k;
    }
    return VL_RESERVED_NONE;
  }

  vl_reserved_kind_t k = vl_reserved_kind_(token);
  if (k != VL_RESERVED_NONE) {
    *form = VL_PARSE_RESERVED_BARE;
    return k;
  }

  const char *p = vl_reserved_strip_dashes_(token);
  if (p[0] == '?' && p[1] != '\0') { /* GCOVR_EXCL_BR_LINE: bare ? short-circuit */
    *form = VL_PARSE_RESERVED_JOINED;
    *target = p + 1;
    return VL_RESERVED_HELP;
  }
  return VL_RESERVED_NONE;
}
