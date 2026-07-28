#include "test_support.h"

valve_t *parser_(const vl_option_t *const *options, size_t option_count) {
  vl_executable_t settings = {
      .options = options,
      .option_count = option_count,
  };
  return vl_create(&settings);
}

valve_t *parser_with_form_(const vl_option_t *const *options,
                           size_t option_count, vl_assign_t assign) {
  vl_executable_t settings = {
      .options = options,
      .option_count = option_count,
      .assign = assign,
  };
  return vl_create(&settings);
}
