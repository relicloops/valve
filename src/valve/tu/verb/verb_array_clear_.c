#include "../../private.h"

#include <stdlib.h>

void verb_array_clear_(valve_verb_t *verbs, size_t count) {
  if (!verbs)
    return;

  for (size_t i = 0; i < count; ++i) {
    vl_option_t *options = verbs[i].options;

    free(verbs[i].name);
    free(verbs[i].description);
    free(verbs[i].usage);
    for (size_t j = 0; j < verbs[i].option_count; ++j) {
      free((char *)options[j].name);
      free((char *)options[j].description);
      free((char *)options[j].usage);
    }
    free(options);

    verb_array_clear_(verbs[i].verbs, verbs[i].verb_count);
    free(verbs[i].verbs);
  }
}

void vl_verbs_clear_(valve_t *v) {
  if (!v)
    return;

  verb_array_clear_(v->verbs_, v->verb_count_);
  free(v->verbs_);
  v->verbs_ = NULL;
  v->verb_count_ = 0;
  v->active_verb_ = NULL;
  v->active_subverb_ = NULL;
}
