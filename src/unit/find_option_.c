#include "../private.h"

#include <string.h>

static const vl_option_t *option_find_in_(const vl_option_t *options,
                                          size_t count, const char *name) {
  if (!options || !name) /* GCOVR_EXCL_BR_LINE: callers never pass null name; null options only with count 0 */
    return NULL;

  for (size_t i = 0; i < count; ++i) {
    if (strcmp(options[i].name, name) == 0) {
      return &options[i];
    }
  }

  return NULL;
}

static const vl_option_t *option_find_n_in_(const vl_option_t *options,
                                            size_t count, const char *name,
                                            size_t len) {
  if (!options || !name) /* GCOVR_EXCL_BR_LINE: callers never pass null name */
    return NULL;

  for (size_t i = 0; i < count; ++i) {
    if (strlen(options[i].name) == len &&
        strncmp(options[i].name, name, len) == 0) {
      return &options[i];
    }
  }

  return NULL;
}

static const vl_option_t *option_find_short_in_(const vl_option_t *options,
                                                size_t count, char short_name) {
  if (!options || short_name == '\0') /* GCOVR_EXCL_BR_LINE: null options only with count 0 */
    return NULL;

  for (size_t i = 0; i < count; ++i) {
    if (options[i].short_name == short_name) {
      return &options[i];
    }
  }

  return NULL;
}

const vl_option_t *vl_option_find_(const valve_t *v, const char *name) {
  const vl_option_t *opt = NULL;

  if (!v || !name)
    return NULL;

  if (v->active_subverb_) {
    opt = option_find_in_(v->active_subverb_->options,
                          v->active_subverb_->option_count, name);
    if (opt)
      return opt;
  }

  if (v->active_verb_) {
    opt = option_find_in_(v->active_verb_->options,
                          v->active_verb_->option_count, name);
    if (opt)
      return opt;
  }

  return option_find_in_(v->options_, v->option_count_, name);
}

const vl_option_t *vl_option_find_n_(const valve_t *v, const char *name,
                                     size_t len) {
  const vl_option_t *opt = NULL;

  if (!v || !name)
    return NULL;

  if (v->active_subverb_) {
    opt = option_find_n_in_(v->active_subverb_->options,
                            v->active_subverb_->option_count, name, len);
    if (opt)
      return opt;
  }

  if (v->active_verb_) {
    opt = option_find_n_in_(v->active_verb_->options,
                            v->active_verb_->option_count, name, len);
    if (opt)
      return opt;
  }

  return option_find_n_in_(v->options_, v->option_count_, name, len);
}

const vl_option_t *vl_option_find_short_(const valve_t *v, char short_name) {
  const vl_option_t *opt = NULL;

  if (!v || short_name == '\0')
    return NULL;

  if (v->active_subverb_) {
    opt = option_find_short_in_(v->active_subverb_->options,
                                v->active_subverb_->option_count, short_name);
    if (opt) {
      return opt;
    }
  }

  if (v->active_verb_) {
    opt = option_find_short_in_(v->active_verb_->options,
                                v->active_verb_->option_count, short_name);
    if (opt) {
      return opt;
    }
  }

  return option_find_short_in_(v->options_, v->option_count_, short_name);
}

const valve_verb_t *verb_find_in_(const valve_verb_t *verbs, size_t count,
                                  const char *name) {
  if (!verbs || !name) /* GCOVR_EXCL_BR_LINE: null name not passed by callers */
    return NULL;

  for (size_t i = 0; i < count; ++i) {
    if (strcmp(verbs[i].name, name) == 0) {
      return &verbs[i];
    }
  }

  return NULL;
}

const valve_verb_t *vl_verb_find_(const valve_t *v, const char *name) {
  if (!v || !name)
    return NULL;

  return verb_find_in_(v->verbs_, v->verb_count_, name);
}
