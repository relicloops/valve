#include "../../valve_private.h"

#include <string.h>

bool option_collides_with_globals_(const vl_option_t *const *globals,
                                          size_t global_count,
                                          const vl_option_t *const *locals,
                                          size_t local_count) {
  for (size_t i = 0; i < local_count; ++i) {
    if (!locals[i]) /* GCOVR_EXCL_BR_LINE: null local slots uncommon; schema walks counted tables */
      continue;
    if (locals[i]->name) { /* GCOVR_EXCL_BR_LINE: null-name local continues to short check */
      for (size_t j = 0; j < global_count; ++j) {
        if (globals[j] && globals[j]->name &&
            strcmp(locals[i]->name, globals[j]->name) == 0)
          return true;
      }
    }
    if (locals[i]->short_name != '\0') {
      for (size_t j = 0; j < global_count; ++j) {
        if (globals[j] && globals[j]->short_name == locals[i]->short_name)
          return true;
      }
    }
  }
  return false;
}