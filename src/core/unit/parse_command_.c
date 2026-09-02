#include "../valve_private.h"

int vl_parse_command_(valve_t *v, int argc, char **argv, int argv_index) {
  const vl_option_t *option = vl_option_find_command_(v);

  if (!option)
    return 0;

  if (argv_index + 1 >= argc || !argv[argv_index + 1] ||
      argv[argv_index + 1][0] == '\0') {
    return vl_error_add_(v, VL_ERROR_MISSING_VALUE, argv_index, option->name,
                         "missing command after --");
  }

  vl_value_t value = {
      .kind = VL_VALUE_COMMAND,
      .as.command =
          {
              .argv = &argv[argv_index + 1],
              .argc = argc - argv_index - 1,
          },
  };

  return vl_result_set_(v, option, &value, argv_index);
}
