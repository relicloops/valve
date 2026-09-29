#include "../private.h"

bool vl_parse_user_has_option_for_token_(const valve_t *v, const char *token) {
  size_t name_len = 0;
  const char *name = vl_parse_option_name_in_token_(token, &name_len);

  if (name_len == 0) /* GCOVR_EXCL_BR_LINE: empty name after dashes */
    return false; /* GCOVR_EXCL_LINE */

  if (vl_option_find_n_(v, name, name_len))
    return true;

  if (name_len == 1 && vl_option_find_short_(v, name[0])) /* GCOVR_EXCL_BR_LINE: reserved shorts rejected by schema */
    return true; /* GCOVR_EXCL_LINE */

  return false;
}
