#include "../private.h"

bool vl_parse_consumable_next_value_(int argc, char **argv, int index, const vl_option_t *opt) {

  if (index + 1 >= argc || !argv[index + 1]) {
    return false;
  }

  const char *next = argv[index + 1];
  if (!vl_parse_looks_like_option_(next)) {
    return true;
  }
  if (vl_parse_accepts_numeric_value_(opt) && vl_parse_looks_like_negative_number_(next)) {
    return true;
  }

  return false;
}
