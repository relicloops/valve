#include "../../private.h"

size_t option_conflict_count_(const vl_option_t *option) {
  if (!option || !option->conflicts)
    return 0;

  if (option->conflict_count > 0)
    return option->conflict_count;

  size_t count = 0;
  while (option->conflicts[count])
    ++count;
  return count;
}
