#include "../private.h"
#include "color.h"

#include <stdio.h>
#include <string.h>

#define VL_HELP_DEFAULT_LOGO "❅"

static const char *logo_(const valve_t *v) {
  return v->logo_ ? v->logo_ : VL_HELP_DEFAULT_LOGO;
}

static const char *c_(const char *seq) { return vl_color_for(stdout, seq); }

static void banner_(const valve_t *v) {
  const char *name = v->program_name_ ? v->program_name_ : PROGRAM_NAME;
  const char *ver = v->program_version_ ? v->program_version_ : PROGRAM_VERSION;

  printf("%s%s%s %s %s%s%s", c_(VAL_FG_RED), logo_(v), c_(VAL_RESET), name,
         c_(VAL_DIM), ver, c_(VAL_RESET));
  if (v->description_)
    printf(" — %s", v->description_);
  putchar('\n');
}

static void print_option_(const valve_t *v, const vl_option_t *opt) {
  if (!opt || !opt->name) /* GCOVR_EXCL_BR_LINE: null opt not passed; null name exercised via overview tables */
    return;

  printf("  %s", c_(VAL_BOLD));
  option_usage_print_(v, opt);
  printf("%s", c_(VAL_RESET));
  option_annotations_print_(v, opt);
  putchar('\n');

  if (opt->usage)
    printf("      %s\n", opt->usage);
  if (opt->description)
    printf("      %s\n", opt->description);
}

static void print_options_(const valve_t *v, const vl_option_t *options,
                           size_t count,
                           const char *heading) {
  if (count == 0)
    return;

  printf("\n%s:\n", heading);
  for (size_t i = 0; i < count; ++i) {
    if (options[i].value != VL_OPTION_VALUE_COMMAND)
      print_option_(v, &options[i]);
  }
  for (size_t i = 0; i < count; ++i) {
    if (options[i].value == VL_OPTION_VALUE_COMMAND)
      print_option_(v, &options[i]);
  }
}

static void print_subverbs_(const valve_verb_t *verb) {
  if (verb->verb_count == 0)
    return;

  printf("\nsub-verbs:\n");
  for (size_t i = 0; i < verb->verb_count; ++i) {
    const valve_verb_t *sub = &verb->verbs[i];
    printf("  %s%s%s\n", c_(VAL_BOLD), sub->name ? sub->name : "",
           c_(VAL_RESET));
    if (sub->description)
      printf("      %s\n", sub->description);
  }
}

/* ---- targeted cards -------------------------------------------------- */

static void print_verb_card_(const valve_t *v, const valve_verb_t *verb) {
  banner_(v);
  printf("\n%s▸%s verb %s%s%s", c_(VAL_FG_CYAN), c_(VAL_RESET), c_(VAL_BOLD),
         verb->name ? verb->name : "", c_(VAL_RESET));
  if (verb->description)
    printf(" — %s", verb->description);
  putchar('\n');

  if (verb->usage)
    printf("\nusage:\n  %s\n", verb->usage);
  print_subverbs_(verb);
  print_options_(v, verb->options, verb->option_count, "options");
}

static void print_subverb_card_(const valve_t *v, const valve_verb_t *verb,
                                const valve_verb_t *sub) {
  char heading[80];

  banner_(v);
  printf("\n%s▸%s %s %s%s%s", c_(VAL_FG_CYAN), c_(VAL_RESET),
         verb && verb->name ? verb->name : "", c_(VAL_BOLD), /* GCOVR_EXCL_BR_LINE: null verb/name */
         sub->name ? sub->name : "", c_(VAL_RESET));
  if (sub->description)
    printf(" — %s", sub->description);
  putchar('\n');

  if (sub->usage)
    printf("\nusage:\n  %s\n", sub->usage);
  print_options_(v, sub->options, sub->option_count, "options");
  if (verb && verb->option_count > 0) { /* GCOVR_EXCL_BR_LINE: null verb short-circuit */
    snprintf(heading, sizeof heading, "shared %s options",
             verb->name ? verb->name : ""); /* GCOVR_EXCL_BR_LINE */
    print_options_(v, verb->options, verb->option_count, heading);
  }
}

static void print_option_card_(const valve_t *v,
                               const vl_help_internal_t *res) {
  const char *verb = res->verb ? res->verb->name : NULL;
  const char *sub = res->subverb ? res->subverb->name : NULL;
  char owner[96];

  if (verb && sub)
    snprintf(owner, sizeof owner, "%s %s", verb, sub);
  else if (verb)
    snprintf(owner, sizeof owner, "%s", verb);
  else
    snprintf(owner, sizeof owner, "global");

  banner_(v);
  printf("\n%s▸%s %s option %s", c_(VAL_FG_CYAN), c_(VAL_RESET), owner,
         c_(VAL_BOLD));
  if (res->option->value == VL_OPTION_VALUE_COMMAND)
    printf("-- <command> [args…]");
  else
    printf("--%s", res->option->name);
  printf("%s\n\n", c_(VAL_RESET));
  print_option_(v, res->option);
}

static void print_group_members_(const valve_t *v, const vl_option_t *opts,
                                 size_t count,
                                 const char *group, size_t glen) {
  for (size_t i = 0; i < count; ++i) {
    const char *n = opts[i].name;
    if (n && strncmp(n, group, glen) == 0 && n[glen] == '.') /* GCOVR_EXCL_BR_LINE: null name / non-prefix */
      print_option_(v, &opts[i]);
  }
}

static void print_group_card_(const valve_t *v, const char *group) {
  size_t glen = strlen(group);

  banner_(v);
  printf("\n%s▸%s group %s%s%s\n\n", c_(VAL_FG_CYAN), c_(VAL_RESET),
         c_(VAL_BOLD), group, c_(VAL_RESET));
  print_group_members_(v, v->options_, v->option_count_, group, glen);
  for (size_t c = 0; c < v->verb_count_; ++c) {
    const valve_verb_t *verb = &v->verbs_[c];
    print_group_members_(v, verb->options, verb->option_count, group, glen);
    for (size_t s = 0; s < verb->verb_count; ++s) {
      const valve_verb_t *sub = &verb->verbs[s];
      print_group_members_(v, sub->options, sub->option_count, group, glen);
    }
  }
}

static bool print_targeted_help_(const valve_t *v, const char *target) {
  vl_help_internal_t res = {0};

  if (!vl_help_resolve_internal_(v, target, &res))
    return false;

  switch (res.kind) { /* GCOVR_EXCL_BR_LINE: NONE/default unreachable on successful resolve */
  case VL_HELP_VERB:
    print_verb_card_(v, res.verb);
    return true;
  case VL_HELP_SUBVERB:
    print_subverb_card_(v, res.verb, res.subverb);
    return true;
  case VL_HELP_OPTION:
    print_option_card_(v, &res);
    return true;
  case VL_HELP_GROUP:
    print_group_card_(v, res.group);
    return true;
  case VL_HELP_NONE: /* GCOVR_EXCL_LINE: resolve never yields NONE on success */
  default: /* GCOVR_EXCL_BR_LINE: defensive default */
    return false; /* GCOVR_EXCL_LINE */
  }
}

/* ---- overview -------------------------------------------------------- */

static void print_overview_(const valve_t *v) {
  banner_(v);

  if (v->usage_)
    printf("\nusage:\n  %s\n", v->usage_);

  print_options_(v, v->options_, v->option_count_, "global options");

  if (v->verb_count_ > 0) {
    printf("\nverbs:\n");
    for (size_t i = 0; i < v->verb_count_; ++i) {
      const valve_verb_t *verb = &v->verbs_[i];
      printf("  %s▸%s %s%s%s", c_(VAL_FG_CYAN), c_(VAL_RESET), c_(VAL_BOLD),
             verb->name ? verb->name : "", c_(VAL_RESET));
      if (verb->description)
        printf(" — %s", verb->description);
      putchar('\n');
      if (verb->usage)
        printf("        %s%s%s\n", c_(VAL_DIM), verb->usage, c_(VAL_RESET));
    }
    printf("\n  %s↳%s targeted: --help=<verb>, --help=<verb>.<sub-verb>, "
           "--help=<group>\n",
           c_(VAL_DIM), c_(VAL_RESET));
  }
  printf("  %s↳%s reserved: --help  --version  --valve\n", c_(VAL_DIM),
         c_(VAL_RESET));
}

void vl_help_print_default_(const valve_t *v) {
  if (!v)
    return;

  if (v->help_target_ && print_targeted_help_(v, v->help_target_))
    return;

  if (v->active_subverb_) {
    print_subverb_card_(v, v->active_verb_, v->active_subverb_);
    return;
  }
  if (v->active_verb_) {
    print_verb_card_(v, v->active_verb_);
    return;
  }

  print_overview_(v);
}
