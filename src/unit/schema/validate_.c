#include "../../private.h"

/* Validates one verb entry, recursing into its sub-verb table: option
 * sanity, collisions with globals and (for sub-verbs) the owning verb. */
static bool verb_entry_valid_(const vl_option_t *const *globals,
                              size_t global_count,
                              const vl_option_t *const *parents,
                              size_t parent_count, const vl_verb_t *verb,
                              bool allow_override) {
  if (verb->option_count > 0 && !verb->options) /* GCOVR_EXCL_BR_LINE: count-without-table rejected; both arms hard to pair in one run */
    return false;

  size_t local_count = option_count_(verb->options, verb->option_count);

  if (options_command_count_(globals, global_count) +
          options_command_count_(parents, parent_count) +
          options_command_count_(verb->options, local_count) >
      1)
    return false;

  if (options_have_invalid_(verb->options, local_count))
    return false;
  if (options_have_duplicate_long_(verb->options, local_count))
    return false;
  if (options_have_duplicate_short_(verb->options, local_count))
    return false;
  if (!options_conflicts_valid_(globals, global_count, parents, parent_count,
                                verb->options, local_count))
    return false;
  if (!options_requirements_valid_(globals, global_count, parents,
                                   parent_count, verb->options, local_count))
    return false;
  if (option_collides_with_globals_(globals, global_count, verb->options,
                                    local_count))
    return false;
  if (!allow_override && options_use_reserved_(verb->options, local_count))
    return false;

  size_t sub_count = verb_table_count_(verb->verbs, verb->verb_count);

  if (sub_count == 0)
    return true;

  if (verbs_have_invalid_(verb->verbs, sub_count))
    return false;
  if (verbs_have_duplicate_(verb->verbs, sub_count))
    return false;
  if (!allow_override && verbs_use_reserved_(verb->verbs, sub_count)) /* GCOVR_EXCL_BR_LINE: allow_override true skips; false arm covered */
    return false;

  for (size_t i = 0; i < sub_count; ++i) {
    size_t sub_options =
        option_count_(verb->verbs[i]->options, verb->verbs[i]->option_count);

    if (option_collides_with_globals_(verb->options, local_count,
                                      verb->verbs[i]->options, sub_options))
      return false;
    if (!verb_entry_valid_(globals, global_count, verb->options, local_count,
                           verb->verbs[i], allow_override))
      return false;
  }

  return true;
}

bool vl_schema_validate_(const vl_executable_t *settings){

  if (!settings)
    return true;

  if (!assign_valid_(settings->assign))
    return false;
  if (settings->option_count > 0 && !settings->options)
    return false;
  if (settings->verb_count > 0 && !settings->verbs)
    return false;

  size_t global_count =
      option_count_(settings->options, settings->option_count);
  size_t verb_count = verb_count_(settings);

  if (options_have_invalid_(settings->options, global_count))
    return false;
  if (options_command_count_(settings->options, global_count) > 1)
    return false;
  if (options_have_duplicate_long_(settings->options, global_count))
    return false;
  if (options_have_duplicate_short_(settings->options, global_count))
    return false;
  if (!options_conflicts_valid_(nullptr, 0, nullptr, 0, settings->options,
                                global_count))
    return false;
  if (!options_requirements_valid_(nullptr, 0, nullptr, 0, settings->options,
                                   global_count))
    return false;
  if (verbs_have_invalid_(settings->verbs, verb_count))
    return false;
  if (verbs_have_duplicate_(settings->verbs, verb_count))
    return false;

  const bool allow_override =
      (settings->behavior & VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED) != 0;
  if (!allow_override) {
    if (options_use_reserved_(settings->options, global_count))
      return false;
    if (verbs_use_reserved_(settings->verbs, verb_count))
      return false;
  }

  for (size_t i = 0; i < verb_count; ++i) {
    if (!verb_entry_valid_(settings->options, global_count, nullptr, 0,
                           settings->verbs[i], allow_override))
      return false;
  }

  return true;

}
