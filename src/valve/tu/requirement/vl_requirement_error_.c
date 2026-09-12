#include "../../private.h"

#include <stdio.h>
#include <stdlib.h>

int vl_requirement_error_(valve_t *v, const vl_option_t *source,
                          const vl_result_t *source_result,
                          const vl_option_t *target) {
  char *label = option_label_(target);
  if (!label)
    return -1;

  int length = snprintf(nullptr, 0, "requires %s", label);
  if (length < 0) { /* GCOVR_EXCL_BR_LINE: snprintf size query failure */
    free(label); /* GCOVR_EXCL_LINE */
    return -1;   /* GCOVR_EXCL_LINE */
  }

  char *message = malloc((size_t)length + 1);
  if (!message) {
    free(label);
    return -1;
  }
  (void)snprintf(message, (size_t)length + 1, "requires %s", label);

  int rc = vl_error_add_(v, VL_ERROR_UNSATISFIED_REQUIREMENT,
                         source_result->argv_index, source->name, message);
  free(message);
  free(label);
  return rc;
}
