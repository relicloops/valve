#include "../private.h"

bool vl_parse_accepts_numeric_value_(const vl_option_t *opt) {
  if (!opt) /* GCOVR_EXCL_BR_LINE: null guard */
    return false; /* GCOVR_EXCL_LINE */
  vl_option_value_t vk = vl_parse_effective_scalar_value_(opt);
  return vk == VL_OPTION_VALUE_INT ||
         vk == VL_OPTION_VALUE_DOUBLE ||
         vk == VL_OPTION_VALUE_NUMBER ||
         vk == VL_OPTION_VALUE_AUTO;
}
