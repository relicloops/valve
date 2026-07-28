#include "../../valve_private.h"

#include <stdio.h>
#include <stdlib.h>

int vl_conflict_error_(valve_t *v, const vl_option_t *later,
                       const vl_result_t *later_result,
                       const vl_option_t *earlier) {
  char *label = option_label_(earlier);
  if (!label)
    return -1;

  int length = snprintf(nullptr, 0, "conflicts with %s", label);
  if (length < 0) { /* GCOVR_EXCL_BR_LINE: snprintf size query failure */
    free(label); /* GCOVR_EXCL_LINE */
    return -1;   /* GCOVR_EXCL_LINE */
  }

  char *message = malloc((size_t)length + 1);
  if (!message) {
    free(label);
    return -1;
  }
  (void)snprintf(message, (size_t)length + 1, "conflicts with %s", label);

  int rc = vl_error_add_(v, VL_ERROR_CONFLICTING_OPTION,
                         later_result->argv_index, later->name, message);
  free(message);
  free(label);
  return rc;
}
