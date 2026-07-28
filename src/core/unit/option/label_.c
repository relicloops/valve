#include "../../valve_private.h"

#include <stdio.h>
#include <stdlib.h>

char *option_label_(const vl_option_t *option) {
  if (!option)
    return nullptr;

  int length;
  if ((option->type & VL_OPT_TYPE_TOGGLE) && option->toggle_ref) {
    length = snprintf(nullptr, 0, "--enable-%s | --disable-%s",
                      option->toggle_ref, option->toggle_ref);
  } else if (option->type & VL_OPT_TYPE_LONG) {
    length = snprintf(nullptr, 0, "--%s", option->name);
  } else if (option->type & VL_OPT_TYPE_SHORT) {
    length = snprintf(nullptr, 0, "-%c", option->short_name);
  } else {
    return nullptr;
  }

  if (length < 0) /* GCOVR_EXCL_BR_LINE: snprintf size query failure */
    return nullptr; /* GCOVR_EXCL_LINE */

  char *label = malloc((size_t)length + 1);
  if (!label)
    return nullptr;

  if ((option->type & VL_OPT_TYPE_TOGGLE) && option->toggle_ref) { /* GCOVR_EXCL_BR_LINE: toggle_ref null after type check rare */
    (void)snprintf(label, (size_t)length + 1, "--enable-%s | --disable-%s",
                   option->toggle_ref, option->toggle_ref);
  } else if (option->type & VL_OPT_TYPE_LONG) {
    (void)snprintf(label, (size_t)length + 1, "--%s", option->name);
  } else {
    (void)snprintf(label, (size_t)length + 1, "-%c", option->short_name);
  }
  return label;
}
