#include "../private.h"

#include <string.h>

static const vl_option_t *opt_by_name_(const vl_option_t *const *opts,
                                       size_t count, const char *name) {
  for (size_t i = 0; i < count; ++i)
    if (opts[i] && opts[i]->name && strcmp(opts[i]->name, name) == 0)
      return opts[i];
  return nullptr;
}

static const vl_verb_t *verb_by_name_(const vl_verb_t *const *verbs,
                                      size_t count, const char *name) {
  for (size_t i = 0; i < count; ++i)
    if (verbs[i] && verbs[i]->name && strcmp(verbs[i]->name, name) == 0) /* GCOVR_EXCL_BR_LINE: null verb/name slots rare in schema tables */
      return verbs[i];
  return nullptr;
}

static bool group_in_(const vl_option_t *const *opts, size_t count,
                      const char *group, size_t glen) {
  for (size_t i = 0; i < count; ++i) {
    const char *n = opts[i] ? opts[i]->name : nullptr;
    if (n && strncmp(n, group, glen) == 0 && n[glen] == '.') /* GCOVR_EXCL_BR_LINE: null name / non-dot short-circuit */
      return true;
  }
  return false;
}

/* Exact option name across globals, every verb, and every sub-verb. */
static bool option_anywhere_(const vl_verb_t *const *verbs, size_t vc,
                             const vl_option_t *const *globals, size_t gc,
                             const char *target, vl_help_resolution_t *out) {
  const vl_option_t *opt = opt_by_name_(globals, gc, target);
  if (opt) {
    *out = (vl_help_resolution_t){.kind = VL_HELP_OPTION, .option = opt};
    return true;
  }
  for (size_t c = 0; c < vc; ++c) {
    const vl_verb_t *verb = verbs[c];
    if (!verb) /* GCOVR_EXCL_BR_LINE: null verb slots rare in schema tables */
      continue;
    size_t voc = option_count_(verb->options, verb->option_count);
    opt = opt_by_name_(verb->options, voc, target);
    if (opt) {
      *out = (vl_help_resolution_t){
          .kind = VL_HELP_OPTION, .verb = verb, .option = opt};
      return true;
    }
    size_t svc = verb_table_count_(verb->verbs, verb->verb_count);
    for (size_t s = 0; s < svc; ++s) {
      const vl_verb_t *sub = verb->verbs[s];
      if (!sub) /* GCOVR_EXCL_BR_LINE: null sub-verb slots rare */
        continue;
      size_t soc = option_count_(sub->options, sub->option_count);
      opt = opt_by_name_(sub->options, soc, target);
      if (opt) {
        *out = (vl_help_resolution_t){.kind = VL_HELP_OPTION,
                                      .verb = verb,
                                      .subverb = sub,
                                      .option = opt};
        return true;
      }
    }
  }
  return false;
}

/* The sole sub-verb of that bare name across every top-level verb, or NULL
 * when zero or more than one verb owns it (ambiguous, e.g. `list`). */
static const vl_verb_t *unique_subverb_(const vl_verb_t *const *verbs,
                                        size_t vc, const char *name,
                                        const vl_verb_t **owner) {
  const vl_verb_t *found = nullptr;
  size_t matches = 0;

  for (size_t c = 0; c < vc; ++c) {
    const vl_verb_t *cand = verbs[c];
    if (!cand) /* GCOVR_EXCL_BR_LINE: null verb slots rare in schema tables */
      continue;
    size_t candsvc = verb_table_count_(cand->verbs, cand->verb_count);
    const vl_verb_t *sub = verb_by_name_(cand->verbs, candsvc, name);
    if (sub) {
      found = sub;
      *owner = cand;
      ++matches;
    }
  }

  return matches == 1 ? found : nullptr;
}

static bool group_anywhere_(const vl_verb_t *const *verbs, size_t vc,
                            const vl_option_t *const *globals, size_t gc,
                            const char *group, size_t glen) {
  if (group_in_(globals, gc, group, glen))
    return true;
  for (size_t c = 0; c < vc; ++c) {
    const vl_verb_t *verb = verbs[c];
    if (!verb) /* GCOVR_EXCL_BR_LINE: null verb slots rare in schema tables */
      continue;
    size_t voc = option_count_(verb->options, verb->option_count);
    if (group_in_(verb->options, voc, group, glen))
      return true;
    size_t svc = verb_table_count_(verb->verbs, verb->verb_count);
    for (size_t s = 0; s < svc; ++s) {
      const vl_verb_t *sub = verb->verbs[s];
      if (!sub) /* GCOVR_EXCL_BR_LINE: null sub-verb slots rare */
        continue;
      size_t soc = option_count_(sub->options, sub->option_count);
      if (group_in_(sub->options, soc, group, glen))
        return true;
    }
  }
  return false;
}

/* The remaining segments from `k` onward, rejoined with dots. vl_path_split
 * copies `target` into `scratch` and turns each dot into NUL, so the same
 * offset in `target` already spans the rest of the path, NUL-terminated. Read
 * from `target` rather than `scratch` so a VL_HELP_GROUP `.group` pointer
 * outlives this frame, as the public contract requires. */
static const char *suffix_from_(const char *target, const char *scratch,
                                const vl_path_t *path, size_t k) {
  return target + (size_t)(path->segments[k] - scratch);
}

bool vl_help_resolve(const vl_verb_t *const *verbs, size_t verb_count,
                     const vl_option_t *const *globals, size_t global_count,
                     const char *target, vl_help_resolution_t *out) {
  if (!target || target[0] == '\0' || !out) /* GCOVR_EXCL_BR_LINE: empty-target arm covered; null out rare */
    return false;

  *out = (vl_help_resolution_t){0};

  size_t vc = verb_table_count_(verbs, verb_count);
  size_t gc = option_count_(globals, global_count);

  /* Structural resolution by dotted segments. Bare tokens prefer verb /
   * unique sub-verb over a same-named option elsewhere (e.g. generate vs
   * skinjo.call --generate). Dotted option names like proxy.lane fall through
   * to option_anywhere_ below. */
  char scratch[128];
  vl_path_t path = {0};
  size_t n = vl_path_split(target, scratch, sizeof(scratch), &path);

  if (n == 1) {
    const vl_verb_t *verb = verb_by_name_(verbs, vc, path.segments[0]);
    if (verb) {
      *out = (vl_help_resolution_t){.kind = VL_HELP_VERB, .verb = verb};
      return true;
    }
    const vl_verb_t *owner = nullptr;
    const vl_verb_t *sub = unique_subverb_(verbs, vc, path.segments[0], &owner);
    if (sub) {
      *out = (vl_help_resolution_t){
          .kind = VL_HELP_SUBVERB, .verb = owner, .subverb = sub};
      return true;
    }
    if (option_anywhere_(verbs, vc, globals, gc, target, out))
      return true;
    if (group_anywhere_(verbs, vc, globals, gc, path.segments[0],
                        strlen(path.segments[0]))) {
      *out = (vl_help_resolution_t){.kind = VL_HELP_GROUP, .group = target};
      return true;
    }
    return false;
  }

  /* Verb-prefixed forms. The option suffix is every segment after the matched
   * verb / sub-verb rejoined, so a dotted option name (disposition.will) is
   * reachable through its owner. Scopes are tried innermost first. */
  if (n >= 2) {
    const vl_verb_t *verb = verb_by_name_(verbs, vc, path.segments[0]);
    if (verb) {
      size_t svc = verb_table_count_(verb->verbs, verb->verb_count);
      const vl_verb_t *sub = verb_by_name_(verb->verbs, svc, path.segments[1]);
      if (sub) {
        if (n == 2) {
          *out = (vl_help_resolution_t){
              .kind = VL_HELP_SUBVERB, .verb = verb, .subverb = sub};
          return true;
        }
        const char *tail = suffix_from_(target, scratch, &path, 2);
        size_t soc = option_count_(sub->options, sub->option_count);
        const vl_option_t *opt = opt_by_name_(sub->options, soc, tail);
        if (opt) {
          *out = (vl_help_resolution_t){.kind = VL_HELP_OPTION,
                                        .verb = verb,
                                        .subverb = sub,
                                        .option = opt};
          return true;
        }
        /* A group prefix only when the tail is a single segment; otherwise
         * .group would name a prefix but point at a longer path. */
        if (n == 3 && group_in_(sub->options, soc, tail, strlen(tail))) {
          *out = (vl_help_resolution_t){.kind = VL_HELP_GROUP,
                                        .verb = verb,
                                        .subverb = sub,
                                        .group = tail};
          return true;
        }
      }

      /* Verb scope: verb.option and verb.group.leaf resolve the same way. */
      const char *tail = suffix_from_(target, scratch, &path, 1);
      size_t voc = option_count_(verb->options, verb->option_count);
      const vl_option_t *opt = opt_by_name_(verb->options, voc, tail);
      if (opt) {
        *out = (vl_help_resolution_t){
            .kind = VL_HELP_OPTION, .verb = verb, .option = opt};
        return true;
      }
      if (n == 2 && group_in_(verb->options, voc, tail, strlen(tail))) {
        *out = (vl_help_resolution_t){
            .kind = VL_HELP_GROUP, .verb = verb, .group = tail};
        return true;
      }
    }
  }

  /* Exact option name anywhere (dotted names like proxy.lane match here). */
  if (option_anywhere_(verbs, vc, globals, gc, target, out))
    return true;

  return false;
}
