#include "../private.h"

#include <string.h>

/* Walk argv[start..argc) as options. `--` hands the tail to the command
 * option and stops. Returns VL_PARSE_CONTINUE when every token was consumed
 * (errors may still have been recorded), 0 when a reserved token fired
 * mid-way, -1 on allocation failure or an invalid reserved target. */
int vl_parse_options_(valve_t *v, int argc, char **argv, int start) {
  for (int i = start; i < argc; ++i) {
    const char *arg = argv[i];
    int rc = 0;

    if (!arg || arg[0] == '\0') {
      rc = vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i, nullptr,
                         "empty argument");
    } else if (strcmp(arg, "--") == 0) {
      rc = vl_parse_command_(v, argc, argv, i);
      if (rc < 0) {
        (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, i, nullptr,
                            "out of memory");
        return -1;
      }
      break;
    } else {
      rc = vl_parse_try_reserved_(v, argc, argv, i, true);
      if (rc != VL_PARSE_CONTINUE)
        return rc;

      if (strncmp(arg, "--", 2) == 0) {
        rc = vl_parse_enable_disable_(v, arg, i);
        if (rc == VL_PARSE_CONTINUE)
          rc = vl_parse_long_option_(v, argc, argv, &i);
      } else if (arg[0] == '-' && arg[1] != '\0') {
        rc = vl_parse_short_option_(v, argc, argv, &i);
      } else {
        rc = vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i, nullptr,
                           "unexpected positional argument");
      }
    }

    if (rc < 0) {
      (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, i, nullptr, "out of memory");
      return -1;
    }
  }

  return VL_PARSE_CONTINUE;
}
