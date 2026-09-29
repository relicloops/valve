#include "../private.h"

#include <string.h>

static const vl_option_t *option_find_in_(const vl_option_t *options,
                                          size_t count, const char *name) {
  if (!options || !name) /* GCOVR_EXCL_BR_LINE: callers never pass null name; null options only with count 0 */
    return nullptr;

  for (size_t i = 0; i < count; ++i) {
    if (strcmp(options[i].name, name) == 0) {
      return &options[i];
    }
  }

  return nullptr;
}

static const vl_option_t *option_find_n_in_(const vl_option_t *options,
                                            size_t count, const char *name,
                                            size_t len) {
  if (!options || !name) /* GCOVR_EXCL_BR_LINE: callers never pass null name */
    return nullptr;

  for (size_t i = 0; i < count; ++i) {
    if (strlen(options[i].name) == len &&
        strncmp(options[i].name, name, len) == 0) {
      return &options[i];
    }
  }

  return nullptr;
}

static const vl_option_t *option_find_short_in_(const vl_option_t *options,
                                                size_t count, char short_name) {
  if (!options || short_name == '\0') /* GCOVR_EXCL_BR_LINE: null options only with count 0 */
    return nullptr;

  for (size_t i = 0; i < count; ++i) {
    if (options[i].short_name == short_name) {
      return &options[i];
    }
  }

  return nullptr;
}

const vl_option_t *vl_option_find_(const valve_t *v, const char *name) {
  const vl_option_t *opt = nullptr;

  if (!v || !name)
    return nullptr;

  if (v->active_action_)
    return option_find_in_(v->active_action_, 1, name);

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
  const vl_option_t *opt = nullptr;

  if (!v || !name)
    return nullptr;

  if (v->active_action_)
    return option_find_n_in_(v->active_action_, 1, name, len);

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
  const vl_option_t *opt = nullptr;

  if (!v || short_name == '\0')
    return nullptr;

  if (v->active_action_)
    return option_find_short_in_(v->active_action_, 1, short_name);

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
    return nullptr;

  for (size_t i = 0; i < count; ++i) {
    if (strcmp(verbs[i].name, name) == 0) {
      return &verbs[i];
    }
  }

  return nullptr;
}

const valve_verb_t *vl_verb_find_(const valve_t *v, const char *name) {
  if (!v || !name)
    return nullptr;

  return verb_find_in_(v->verbs_, v->verb_count_, name);
}

const vl_option_t *vl_option_find_toggle_ref_(const valve_t *v,
                                              const char *ref) {
  if (!v || !ref) /* GCOVR_EXCL_BR_LINE: null guard */
    return nullptr; /* GCOVR_EXCL_LINE */

  if (v->active_action_) {
    /* GCOVR_EXCL_BR_START — action matched by toggle_ref, so this holds */
    if (v->active_action_->toggle_ref &&
        strcmp(v->active_action_->toggle_ref, ref) == 0)
      return v->active_action_;
    /* GCOVR_EXCL_BR_STOP */
    return nullptr; /* GCOVR_EXCL_LINE */
  }

  if (v->active_subverb_) {
    for (size_t i = 0; i < v->active_subverb_->option_count; ++i) {
      const vl_option_t *opt = &v->active_subverb_->options[i];
      if (opt->toggle_ref && strcmp(opt->toggle_ref, ref) == 0) /* GCOVR_EXCL_BR_LINE: null toggle_ref short-circuit */
        return opt;
    }
  }

  if (v->active_verb_) {
    for (size_t i = 0; i < v->active_verb_->option_count; ++i) {
      const vl_option_t *opt = &v->active_verb_->options[i];
      if (opt->toggle_ref && strcmp(opt->toggle_ref, ref) == 0) /* GCOVR_EXCL_BR_LINE: null toggle_ref short-circuit */
        return opt;
    }
  }

  for (size_t i = 0; i < v->option_count_; ++i) {
    /* GCOVR_EXCL_BR_START — null toggle_ref short-circuit */
    if (v->options_[i].toggle_ref &&
        strcmp(v->options_[i].toggle_ref, ref) == 0)
      return &v->options_[i];
    /* GCOVR_EXCL_BR_STOP */
  }

  return nullptr;
}
