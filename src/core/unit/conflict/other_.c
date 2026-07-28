#include "../../valve_private.h"

const vl_option_t *vl_conflict_other_(const valve_conflict_t *conflict,
                                      const vl_option_t *option) {
  if (conflict->first == option)
    return conflict->second;
  if (conflict->second == option)
    return conflict->first;
  return nullptr;
}
