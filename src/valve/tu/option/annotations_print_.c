#include "../../private.h"
#include "valve/color.h"

#include <stdio.h>

void option_annotations_print_(const valve_t *v,
                               const vl_option_t *option) {
  bool have_conflict = false;
  for (size_t i = 0; i < v->conflict_count_; ++i) {
    if (vl_conflict_other_(&v->conflicts_[i], option)) {
      have_conflict = true;
      break;
    }
  }

  bool have_requires = false;
  bool have_required_by = false;
  for (size_t i = 0; i < v->requirement_count_; ++i) {
    const valve_requirement_t *requirement = &v->requirements_[i];
    if (requirement->source == option)
      have_requires = true;
    if (requirement->target == option)
      have_required_by = true;
  }

  if (!option->required && !have_requires && !have_required_by &&
      !have_conflict)
    return;

  printf("  %s[", vl_color_for(stdout, VAL_DIM));
  bool wrote_section = false;
  if (option->required) {
    printf("required");
    wrote_section = true;
  }

  if (have_requires) {
    printf("%srequires: ", wrote_section ? "; " : "");
    bool wrote_label = false;
    for (size_t i = 0; i < v->requirement_count_; ++i) {
      const valve_requirement_t *requirement = &v->requirements_[i];
      /* GCOVR_EXCL_BR_START — seen-duplicate continue */
      if (requirement->source != option ||
          vl_requirement_forward_seen_(v, i, option, requirement->target))
        continue;
      /* GCOVR_EXCL_BR_STOP */
      option_label_print_(requirement->target, wrote_label);
      wrote_label = true;
    }
    wrote_section = true;
  }

  if (have_required_by) {
    printf("%srequired by: ", wrote_section ? "; " : "");
    bool wrote_label = false;
    for (size_t i = 0; i < v->requirement_count_; ++i) {
      const valve_requirement_t *requirement = &v->requirements_[i];
      if (requirement->target != option ||
          vl_requirement_reverse_seen_(v, i, option, requirement->source))
        continue;
      option_label_print_(requirement->source, wrote_label);
      wrote_label = true;
    }
    wrote_section = true;
  }

  if (have_conflict) {
    printf("%sconflicts: ", wrote_section ? "; " : "");
    bool wrote_label = false;
    for (size_t i = 0; i < v->conflict_count_; ++i) {
      const vl_option_t *other =
          vl_conflict_other_(&v->conflicts_[i], option);
      if (!other || vl_conflict_seen_(v, i, option, other)) /* GCOVR_EXCL_BR_LINE: seen-duplicate continue */
        continue;
      option_label_print_(other, wrote_label);
      wrote_label = true;
    }
  }
  printf("]%s", vl_color_for(stdout, VAL_RESET));
}
