#include "../../private.h"

#include <stdlib.h>

void option_array_free_(vl_option_t *options, size_t count) {

  for (size_t i = 0; i < count; ++i) {
    free((char *)options[i].name);
    free((char *)options[i].description);
    free((char *)options[i].usage);
  }
  free(options);
}