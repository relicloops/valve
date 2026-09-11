#include "../private.h"

#include <string.h>

static const vl_option_t *opt_by_name_(const vl_option_t *opts, size_t count,
                                       const char *name) {
  for (size_t i = 0; i < count; ++i)
    if (opts[i].name && strcmp(opts[i].name, name) == 0)
      return &opts[i];
  return nullptr;
}

static const valve_verb_t *verb_by_name_(const valve_verb_t *verbs,
                                         size_t count, const char *name) {
  for (size_t i = 0; i < count; ++i)
    if (verbs[i].name && strcmp(verbs[i].name, name) == 0)
      return &verbs[i];
  return nullptr;
}

static bool group_in_(const vl_option_t *opts, size_t count, const char *group,
                      size_t glen) {
  for (size_t i = 0; i < count; ++i) {
    const char *n = opts[i].name;
    if (n && strncmp(n, group, glen) == 0 && n[glen] == '.') /* GCOVR_EXCL_BR_LINE: null name short-circuit */
      return true;
  }
  return false;
}

/* Exact option name anywhere in the copied tree; fills the owning chain. */
static bool option_anywhere_(const valve_t *v, const char *name,
                             vl_help_internal_t *out) {
  const vl_option_t *opt = opt_by_name_(v->options_, v->option_count_, name);
  if (opt) {
    *out = (vl_help_internal_t){.kind = VL_HELP_OPTION, .option = opt};
    return true;
  }
  for (size_t c = 0; c < v->verb_count_; ++c) {
    const valve_verb_t *verb = &v->verbs_[c];
    opt = opt_by_name_(verb->options, verb->option_count, name);
    if (opt) {
      *out = (vl_help_internal_t){
          .kind = VL_HELP_OPTION, .verb = verb, .option = opt};
      return true;
    }
    for (size_t s = 0; s < verb->verb_count; ++s) {
      const valve_verb_t *sub = &verb->verbs[s];
      opt = opt_by_name_(sub->options, sub->option_count, name);
      if (opt) {
        *out = (vl_help_internal_t){.kind = VL_HELP_OPTION,
                                    .verb = verb,
                                    .subverb = sub,
                                    .option = opt};
        return true;
      }
    }
  }
  return false;
}

/* A `group.*` prefix present in globals, any verb, or any sub-verb. */
static bool group_anywhere_(const valve_t *v, const char *group, size_t glen) {
  if (group_in_(v->options_, v->option_count_, group, glen))
    return true;
  for (size_t c = 0; c < v->verb_count_; ++c) {
    const valve_verb_t *verb = &v->verbs_[c];
    if (group_in_(verb->options, verb->option_count, group, glen))
      return true;
    for (size_t s = 0; s < verb->verb_count; ++s) {
      const valve_verb_t *sub = &verb->verbs[s];
      if (group_in_(sub->options, sub->option_count, group, glen))
        return true;
    }
  }
  return false;
}

/* The sole sub-verb of that bare name across every top-level verb, or NULL
 * when zero or more than one verb owns it (ambiguous, e.g. `list`). */
static const valve_verb_t *unique_subverb_(const valve_t *v, const char *name,
                                           const valve_verb_t **owner) {
  const valve_verb_t *found = nullptr;
  size_t matches = 0;

  for (size_t c = 0; c < v->verb_count_; ++c) {
    const valve_verb_t *cand = &v->verbs_[c];
    const valve_verb_t *sub =
        verb_by_name_(cand->verbs, cand->verb_count, name);
    if (sub) {
      found = sub;
      *owner = cand;
      ++matches;
    }
  }

  return matches == 1 ? found : nullptr;
}

static bool resolve_active_(const valve_t *v, const char *target,
                            vl_help_internal_t *out) {
  if (v->active_subverb_) {
    const vl_option_t *opt = opt_by_name_(v->active_subverb_->options,
                                          v->active_subverb_->option_count,
                                          target);
    if (opt) {
      *out = (vl_help_internal_t){.kind = VL_HELP_OPTION,
                                  .verb = v->active_verb_,
                                  .subverb = v->active_subverb_,
                                  .option = opt};
      return true;
    }
  }
  if (v->active_verb_) {
    const vl_option_t *opt = opt_by_name_(
        v->active_verb_->options, v->active_verb_->option_count, target);
    if (opt) {
      *out = (vl_help_internal_t){
          .kind = VL_HELP_OPTION, .verb = v->active_verb_, .option = opt};
      return true;
    }
    const valve_verb_t *sub = verb_by_name_(
        v->active_verb_->verbs, v->active_verb_->verb_count, target);
    if (sub) {
      *out = (vl_help_internal_t){
          .kind = VL_HELP_SUBVERB, .verb = v->active_verb_, .subverb = sub};
      return true;
    }
  }
  return false;
}

/* The remaining segments from `k` onward, rejoined with dots. vl_path_split
 * copies `target` into `scratch` and turns each dot into NUL, so the same
 * offset in `target` already spans the rest of the path, NUL-terminated. Read
 * from `target` rather than `scratch` so a VL_HELP_GROUP `.group` pointer
 * outlives this frame. Mirrors suffix_from_ in help_resolve.c. */
static const char *suffix_from_(const char *target, const char *scratch,
                                const vl_path_t *path, size_t k) {
  return target + (size_t)(path->segments[k] - scratch);
}

bool vl_help_resolve_internal_(const valve_t *v, const char *target,
                               vl_help_internal_t *out) {
  if (!v || !target || target[0] == '\0' || !out)
    return false;

  *out = (vl_help_internal_t){0};

  /* Active-scope single-token resolution (e.g. `agent --help=list`). */
  if (resolve_active_(v, target, out))
    return true;

  /* Structural resolution by dotted segments. Bare tokens prefer verb /
   * unique sub-verb over a same-named option elsewhere (e.g. generate vs
   * skinjo.call --generate). Dotted option names like proxy.lane fall through
   * to option_anywhere_ below. */
  char scratch[128];
  vl_path_t path = {0};
  size_t n = vl_path_split(target, scratch, sizeof(scratch), &path);

  if (n == 1) {
    const valve_verb_t *verb =
        verb_by_name_(v->verbs_, v->verb_count_, path.segments[0]);
    if (verb) {
      *out = (vl_help_internal_t){.kind = VL_HELP_VERB, .verb = verb};
      return true;
    }
    const valve_verb_t *owner = nullptr;
    const valve_verb_t *sub = unique_subverb_(v, path.segments[0], &owner);
    if (sub) {
      *out = (vl_help_internal_t){
          .kind = VL_HELP_SUBVERB, .verb = owner, .subverb = sub};
      return true;
    }
    if (option_anywhere_(v, target, out))
      return true;
    if (group_anywhere_(v, path.segments[0], strlen(path.segments[0]))) {
      *out = (vl_help_internal_t){.kind = VL_HELP_GROUP, .group = target};
      return true;
    }
    return false;
  }

  /* Verb-prefixed forms. The option suffix is every segment after the matched
   * verb / sub-verb rejoined, so a dotted option name (disposition.will) is
   * reachable through its owner. Scopes are tried innermost first. */
  if (n >= 2) {
    const valve_verb_t *verb =
        verb_by_name_(v->verbs_, v->verb_count_, path.segments[0]);
    if (verb) {
      const valve_verb_t *sub =
          verb_by_name_(verb->verbs, verb->verb_count, path.segments[1]);
      if (sub) {
        if (n == 2) {
          *out = (vl_help_internal_t){
              .kind = VL_HELP_SUBVERB, .verb = verb, .subverb = sub};
          return true;
        }
        const char *tail = suffix_from_(target, scratch, &path, 2);
        const vl_option_t *opt =
            opt_by_name_(sub->options, sub->option_count, tail);
        if (opt) {
          *out = (vl_help_internal_t){.kind = VL_HELP_OPTION,
                                      .verb = verb,
                                      .subverb = sub,
                                      .option = opt};
          return true;
        }
        /* A group prefix only when the tail is a single segment; otherwise
         * .group would name a prefix but point at a longer path. */
        if (n == 3 &&
            group_in_(sub->options, sub->option_count, tail, strlen(tail))) {
          *out = (vl_help_internal_t){.kind = VL_HELP_GROUP,
                                      .verb = verb,
                                      .subverb = sub,
                                      .group = tail};
          return true;
        }
      }

      /* Verb scope: verb.option and verb.group.leaf resolve the same way. */
      const char *tail = suffix_from_(target, scratch, &path, 1);
      const vl_option_t *opt =
          opt_by_name_(verb->options, verb->option_count, tail);
      if (opt) {
        *out = (vl_help_internal_t){
            .kind = VL_HELP_OPTION, .verb = verb, .option = opt};
        return true;
      }
      if (n == 2 &&
          group_in_(verb->options, verb->option_count, tail, strlen(tail))) {
        *out = (vl_help_internal_t){
            .kind = VL_HELP_GROUP, .verb = verb, .group = tail};
        return true;
      }
    }
  }

  /* Exact option name anywhere (dotted names like proxy.lane match here). */
  if (option_anywhere_(v, target, out))
    return true;

  return false;
}
