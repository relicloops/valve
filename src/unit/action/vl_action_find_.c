#include "../../private.h"

#include <string.h>

/* Recognise an executable action in a raw argv token: `--name`, `--name=…`,
 * `-c`, `-c=…`, `--enable-ref` / `--disable-ref`. Only the name part is
 * matched here; form and value rules are applied by the option parsers with
 * lookup narrowed to the returned option. */
const vl_option_t *vl_action_find_(const valve_t *v, const char *token) {
  if (!v || !token || !(v->behavior_ & VL_BEHAVIOR_ACCEPT_NO_VERB))
    return nullptr;
  if (token[0] != '-' || token[1] == '\0')
    return nullptr;

  if (token[1] != '-') {
    for (size_t i = 0; i < v->action_count_; ++i) {
      if (v->actions_[i].short_name == token[1])
        return &v->actions_[i];
    }
    return nullptr;
  }

  const char *name = token + 2;
  const char *ref = nullptr;

  if (strncmp(name, "enable-", 7) == 0)
    ref = name + 7;
  else if (strncmp(name, "disable-", 8) == 0)
    ref = name + 8;
  if (ref && ref[0] != '\0') {
    for (size_t i = 0; i < v->action_count_; ++i) {
      if (v->actions_[i].toggle_ref &&
          strcmp(v->actions_[i].toggle_ref, ref) == 0)
        return &v->actions_[i];
    }
  }

  const char *eq = strchr(name, '=');
  size_t len = eq ? (size_t)(eq - name) : strlen(name);

  if (len == 0)
    return nullptr;

  for (size_t i = 0; i < v->action_count_; ++i) {
    if (strlen(v->actions_[i].name) == len &&
        strncmp(v->actions_[i].name, name, len) == 0)
      return &v->actions_[i];
  }

  return nullptr;
}
