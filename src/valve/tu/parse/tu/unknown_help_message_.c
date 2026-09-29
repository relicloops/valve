#include "../private.h"

#include <stdio.h>
#include <string.h>

/* When a help target does not resolve, suggest dotted forms for any verb whose
 * sub-verb or option is named `target` (the ambiguity `--help=list` hits). */
const char *vl_parse_unknown_help_message_(const valve_t *v, const char *target, char *buf, size_t buflen) {

  int wrote = snprintf(buf, buflen, "unknown help target");
  if (wrote < 0 || (size_t)wrote >= buflen) /* GCOVR_EXCL_BR_LINE: snprintf/buf failure */
  {
    return "unknown help target";
  } /* GCOVR_EXCL_LINE */

  size_t used = (size_t)wrote;
  size_t hints = 0;
  for (size_t c = 0; c < v->verb_count_ && used < buflen; ++c) {
    const valve_verb_t *verb = &v->verbs_[c];
    bool match = false;
    for (size_t s = 0; s < verb->verb_count && !match; ++s) {
      if (verb->verbs[s].name && strcmp(verb->verbs[s].name, target) == 0) /* GCOVR_EXCL_BR_LINE: null sub name */
      {
        match = true;
      }
    }

    for (size_t o = 0; o < verb->option_count && !match; ++o) {
      if (verb->options[o].name &&
          strcmp(verb->options[o].name, target) == 0) /* GCOVR_EXCL_BR_LINE: option_anywhere_ resolves first */
      {
        match = true;
      }
    } /* GCOVR_EXCL_LINE */

    if (!match) {
      continue;
    }

    wrote = snprintf(buf + used, buflen - used, "%s%s.%s", hints == 0 ? "; did you mean " : ", ", verb->name, target);
    if (wrote < 0 || (size_t)wrote >= buflen - used) /* GCOVR_EXCL_BR_LINE: hint buffer exhausted */
    {
      return buf;
    } /* GCOVR_EXCL_LINE */

    used += (size_t)wrote;
    ++hints;
  }

  if (hints && used < buflen) /* GCOVR_EXCL_BR_LINE: buffer-full short-circuit */
  {
    (void)snprintf(buf + used, buflen - used, "?");
  }

  return buf;
}
