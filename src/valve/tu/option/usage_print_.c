#include "../../private.h"

#include <stdio.h>

void option_usage_print_(const valve_t *v, const vl_option_t *option) {
  if (option->value == VL_OPTION_VALUE_COMMAND) {
    printf("-- <command> [args…]");
    return;
  }

  if (option->type & VL_OPT_TYPE_TOGGLE) {
    printf("--enable-%s | --disable-%s", option->toggle_ref,
           option->toggle_ref);
    return;
  }

  const bool bare = option->value == VL_OPTION_VALUE_BOOL;
  const char *assign = v->assign_ == VL_ASSIGN_INLINE ? "=" : " ";

  if (option->type & VL_OPT_TYPE_LONG) {
    printf("--%s", option->name);
    if (!bare)
      printf("%s<%s>", assign, option_value_label_(option));
  }

  if ((option->type & VL_OPT_TYPE_LONG) &&
      (option->type & VL_OPT_TYPE_SHORT))
    printf(", ");

  if (option->type & VL_OPT_TYPE_SHORT) {
    printf("-%c", option->short_name);
    if (!bare)
      printf("%s<%s>", assign, option_value_label_(option));
  }
}
