#include "../../private.h"

/* An action reuses the option shape, so the option validators apply to the
 * embedded option first. The fields that only mean something inside a verb
 * scope are then refused: an action is optional by nature, argv[1] cannot
 * repeat, nothing else parses once it fires, and `--` never follows it. */
static bool action_option_valid_(const vl_option_t *option) {
  if (option->required || option->repeat != VL_OPTION_REPEAT_ERROR)
    return false;
  if (option->conflicts || option->conflict_count > 0)
    return false;
  if (option->requires || option->require_count > 0)
    return false;
  if (option->value == VL_OPTION_VALUE_COMMAND)
    return false;
  return true;
}

bool actions_valid_(const vl_executable_t *settings) {
  if (settings->action_count > 0 && !settings->actions)
    return false;

  size_t count = action_count_(settings->actions, settings->action_count);

  if (count == 0)
    return true;
  if (!(settings->behavior & VL_BEHAVIOR_ACCEPT_NO_VERB))
    return false;

  size_t global_count =
      option_count_(settings->options, settings->option_count);

  for (size_t i = 0; i < count; ++i) {
    const vl_executable_action_t *action = settings->actions[i];

    if (!action || !action->run)
      return false;

    const vl_option_t *const one[] = {&action->option};

    if (options_have_invalid_(one, 1))
      return false;
    if (!action_option_valid_(&action->option))
      return false;
    /* Actions are the executable's own vocabulary: no override flag applies. */
    if (options_use_reserved_(one, 1))
      return false;
    if (option_collides_with_globals_(settings->options, global_count, one, 1))
      return false;
    for (size_t j = 0; j < i; ++j) {
      const vl_option_t *const earlier[] = {&settings->actions[j]->option};

      if (option_collides_with_globals_(earlier, 1, one, 1))
        return false;
    }
  }

  return true;
}
