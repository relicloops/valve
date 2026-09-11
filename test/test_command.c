#include "test_support.h"
#include "fault_alloc.h"
#include "private.h"

/* === ceedling valve sources (auto) === */
TEST_SOURCE_FILE("action_count_.c")
TEST_SOURCE_FILE("actions_valid_.c")
TEST_SOURCE_FILE("annotations_print_.c")
TEST_SOURCE_FILE("array_free_.c")
TEST_SOURCE_FILE("at.c")
TEST_SOURCE_FILE("c_locale_.c")
TEST_SOURCE_FILE("clear.c")
TEST_SOURCE_FILE("collides_with_globals_.c")
TEST_SOURCE_FILE("color.c")
TEST_SOURCE_FILE("command_valid_.c")
TEST_SOURCE_FILE("conflict_count_.c")
TEST_SOURCE_FILE("conflicts_valid_.c")
TEST_SOURCE_FILE("count.c")
TEST_SOURCE_FILE("destroy.c")
TEST_SOURCE_FILE("duration_parse_.c")
TEST_SOURCE_FILE("error_add_.c")
TEST_SOURCE_FILE("error_at.c")
TEST_SOURCE_FILE("error_count.c")
TEST_SOURCE_FILE("errors_foreach.c")
TEST_SOURCE_FILE("errors_print.c")
TEST_SOURCE_FILE("find_.c")
TEST_SOURCE_FILE("find_command_.c")
TEST_SOURCE_FILE("find_option_.c")
TEST_SOURCE_FILE("forward_seen_.c")
TEST_SOURCE_FILE("has.c")
TEST_SOURCE_FILE("have_duplicate_.c")
TEST_SOURCE_FILE("have_duplicate_long_.c")
TEST_SOURCE_FILE("have_duplicate_short_.c")
TEST_SOURCE_FILE("help_print_.c")
TEST_SOURCE_FILE("help_resolve.c")
TEST_SOURCE_FILE("help_resolve_internal_.c")
TEST_SOURCE_FILE("help_target.c")
TEST_SOURCE_FILE("in_.c")
TEST_SOURCE_FILE("kv_parse_.c")
TEST_SOURCE_FILE("label_.c")
TEST_SOURCE_FILE("label_print_.c")
TEST_SOURCE_FILE("mapped_.c")
TEST_SOURCE_FILE("option_count_.c")
TEST_SOURCE_FILE("option_name_valid_.c")
TEST_SOURCE_FILE("options_command_count_.c")
TEST_SOURCE_FILE("options_copy_.c")
TEST_SOURCE_FILE("options_have_invalid_.c")
TEST_SOURCE_FILE("options_use_reserved_.c")
TEST_SOURCE_FILE("other_.c")
TEST_SOURCE_FILE("parse.c")
TEST_SOURCE_FILE("parse_command_.c")
TEST_SOURCE_FILE("path_split.c")
TEST_SOURCE_FILE("repeat_valid_.c")
TEST_SOURCE_FILE("required_check_.c")
TEST_SOURCE_FILE("requirement_count_.c")
TEST_SOURCE_FILE("requirements_valid_.c")
TEST_SOURCE_FILE("reserved.c")
TEST_SOURCE_FILE("reserved_fired.c")
TEST_SOURCE_FILE("reverse_seen_.c")
TEST_SOURCE_FILE("scalar_auto_.c")
TEST_SOURCE_FILE("seen_.c")
TEST_SOURCE_FILE("set_.c")
TEST_SOURCE_FILE("strtod_c_.c")
TEST_SOURCE_FILE("subverb_get.c")
TEST_SOURCE_FILE("table_count_.c")
TEST_SOURCE_FILE("targets_clear.c")
TEST_SOURCE_FILE("type_valid_.c")
TEST_SOURCE_FILE("usage_print_.c")
TEST_SOURCE_FILE("validate_.c")
TEST_SOURCE_FILE("value_label_.c")
TEST_SOURCE_FILE("value_valid_.c")
TEST_SOURCE_FILE("valve.c")
TEST_SOURCE_FILE("valve_print_.c")
TEST_SOURCE_FILE("verb_array_clear_.c")
TEST_SOURCE_FILE("verb_copy_one_.c")
TEST_SOURCE_FILE("verb_count_.c")
TEST_SOURCE_FILE("verb_name_valid_.c")
TEST_SOURCE_FILE("verbs_have_invalid_.c")
TEST_SOURCE_FILE("verbs_use_reserved_.c")
TEST_SOURCE_FILE("version_get.c")
TEST_SOURCE_FILE("version_print_.c")
TEST_SOURCE_FILE("version_string.c")
TEST_SOURCE_FILE("visible_.c")
TEST_SOURCE_FILE("vl_action_find_.c")
TEST_SOURCE_FILE("vl_action_fired.c")
TEST_SOURCE_FILE("vl_actions_clear_.c")
TEST_SOURCE_FILE("vl_actions_copy_.c")
TEST_SOURCE_FILE("vl_conflict_active_.c")
TEST_SOURCE_FILE("vl_conflict_add_.c")
TEST_SOURCE_FILE("vl_conflict_error_.c")
TEST_SOURCE_FILE("vl_conflict_exists_.c")
TEST_SOURCE_FILE("vl_conflicts_check_.c")
TEST_SOURCE_FILE("vl_conflicts_clear_.c")
TEST_SOURCE_FILE("vl_conflicts_copy_.c")
TEST_SOURCE_FILE("vl_conflicts_copy_table_.c")
TEST_SOURCE_FILE("vl_get.c")
TEST_SOURCE_FILE("vl_options_clear_.c")
TEST_SOURCE_FILE("vl_requirement_active_.c")
TEST_SOURCE_FILE("vl_requirement_add_.c")
TEST_SOURCE_FILE("vl_requirement_error_.c")
TEST_SOURCE_FILE("vl_requirement_exists_.c")
TEST_SOURCE_FILE("vl_requirements_check_.c")
TEST_SOURCE_FILE("vl_requirements_clear_.c")
TEST_SOURCE_FILE("vl_requirements_copy_.c")
TEST_SOURCE_FILE("vl_requirements_copy_table_.c")
TEST_SOURCE_FILE("vl_results_clear_.c")
TEST_SOURCE_FILE("vl_verb_get.c")
/* === end ceedling valve sources === */

#include <unistd.h>

static_assert(sizeof(vl_command_t) == sizeof(vl_array_t),
              "command view must not grow the value union");

void setUp(void) {}
void tearDown(void) {
  vl_color_support_reset();
  vl_test_alloc_reset();
}

static valve_t *parser_for_(const vl_option_t *const *options, size_t count) {
  const vl_executable_t settings = {
      .options = options,
      .option_count = count,
      .color = VAL_COLOR_NEVER,
  };
  return vl_create(&settings);
}

static char *capture_parse_(valve_t *v, int argc, char **argv) {
  FILE *out = tmpfile();
  int saved = -1;
  char *text = NULL;
  long size = 0;

  if (!out)
    return NULL;

  fflush(stdout);
  saved = dup(fileno(stdout));
  if (saved < 0 || dup2(fileno(out), fileno(stdout)) < 0)
    goto done;

  if (vl_parse(v, argc, argv) != 0)
    goto restore;

restore:
  fflush(stdout);
  if (saved >= 0) {
    (void)dup2(saved, fileno(stdout));
    close(saved);
    saved = -1;
  }

  if (fseek(out, 0, SEEK_END) != 0)
    goto done;
  size = ftell(out);
  if (size < 0 || fseek(out, 0, SEEK_SET) != 0)
    goto done;

  text = malloc((size_t)size + 1);
  if (!text)
    goto done;
  if (fread(text, 1, (size_t)size, out) != (size_t)size) {
    free(text);
    text = NULL;
    goto done;
  }
  text[size] = '\0';

done:
  if (saved >= 0) {
    (void)dup2(saved, fileno(stdout));
    close(saved);
  }
  fclose(out);
  return text;
}

void test_command_tail_is_borrowed_and_exec_ready(void) {
  vl_command_t target = {0};
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
      .required = true,
      .data = &target,
      .target = VL_TARGET_COMMAND,
  };
  const vl_option_t *const options[] = {&command, NULL};
  const vl_executable_t settings = {
      .options = options,
      .option_count = 1,
  };
  char *argv[] = {(char *)"valve", (char *)"--", (char *)"generator",
                  (char *)"cron", (char *)"list", NULL};
  valve_t *v = vl_create(&settings);

  EXPECT(v != NULL, "command parser created");
  EXPECT(vl_parse(v, 5, argv) == 0, "command tail parsed");
  EXPECT(target.argv == &argv[2] && target.argc == 3,
         "direct target borrows the tail");
  EXPECT(target.argv[3] == NULL, "borrowed tail remains NULL-terminated");

  const vl_value_t *value = vl_get(v, "command");
  EXPECT(value && value->kind == VL_VALUE_COMMAND,
         "command result has command kind");
  EXPECT(value->as.command.argv == target.argv &&
             value->as.command.argc == target.argc,
         "result and target share the borrowed view");
  EXPECT(vl_result_at(v, 0) && vl_result_at(v, 0)->argv_index == 1,
         "command result records the end marker index");

  vl_destroy(v);
  EXPECT(target.argv == &argv[2] && target.argc == 3,
         "direct target survives parser destruction");
  vl_targets_clear(&settings);
  EXPECT(target.argv == NULL && target.argc == 0,
         "direct command target clears to zero");
}

void test_command_value_target_survives_destroy(void) {
  vl_value_t target = {0};
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
      .data = &target,
      .target = VL_TARGET_VALUE,
  };
  const vl_option_t *const options[] = {&command, NULL};
  const vl_executable_t settings = {.options = options, .option_count = 1};
  char *argv[] = {(char *)"valve", (char *)"--", (char *)"oracle",
                  (char *)"draw", NULL};
  valve_t *v = vl_create(&settings);

  EXPECT(v != NULL && vl_parse(v, 4, argv) == 0,
         "command parses into value target");
  vl_destroy(v);
  EXPECT(target.kind == VL_VALUE_COMMAND && target.as.command.argv == &argv[2] &&
             target.as.command.argc == 2,
         "value target keeps borrowed command after destroy");
  vl_targets_clear(&settings);
  EXPECT(target.raw == NULL && target.as.command.argv == NULL,
         "value command target clears without freeing argv");
}

void test_command_tail_is_opaque(void) {
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  const vl_option_t *const options[] = {&command, NULL};
  char *argv[] = {(char *)"valve", (char *)"--", (char *)"--help",
                  (char *)"--enable-debug", (char *)"--", NULL};
  valve_t *v = parser_for_(options, 1);

  EXPECT(v != NULL && vl_parse(v, 5, argv) == 0, "opaque tail parses");
  EXPECT(!vl_reserved_fired(v), "reserved tokens in tail are not fired");
  EXPECT(vl_get(v, "command") && vl_get(v, "command")->as.command.argc == 3,
         "all opaque tail tokens are retained");
  vl_destroy(v);
}

void test_empty_command_tail_is_rejected(void) {
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  const vl_option_t *const options[] = {&command, NULL};
  char *empty_tail[] = {(char *)"valve", (char *)"--", NULL};
  char *empty_first[] = {(char *)"valve", (char *)"--", (char *)"", NULL};
  char *null_first[] = {(char *)"valve", (char *)"--", NULL, NULL};
  char **cases[] = {empty_tail, empty_first, null_first};
  int counts[] = {2, 3, 3};

  for (size_t i = 0; i < 3; ++i) {
    valve_t *v = parser_for_(options, 1);
    EXPECT(v != NULL && vl_parse(v, counts[i], cases[i]) == -1,
           "empty command form rejected");
    const vl_error_t *error = vl_error_at(v, 0);
    EXPECT(error && error->code == VL_ERROR_MISSING_VALUE &&
               error->argv_index == 1 && strcmp(error->key, "command") == 0 &&
               strcmp(error->message, "missing command after --") == 0,
           "empty command reports the command-specific error");
    vl_destroy(v);
  }
}

void test_unowned_tail_keeps_existing_behavior_without_sentinel(void) {
  char *argv[] = {(char *)"valve", (char *)"--", (char *)"ignored"};
  valve_t *v = vl_create(NULL);

  EXPECT(v != NULL && vl_parse(v, 3, argv) == 0,
         "ordinary schema still discards an unowned tail");
  EXPECT(vl_result_count(v) == 0, "unowned tail creates no result");
  vl_destroy(v);
}

void test_command_name_forms_are_disabled(void) {
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  const vl_option_t *const options[] = {&command, NULL};
  char *inline_form[] = {(char *)"valve", (char *)"--command=x", NULL};
  char *bare_form[] = {(char *)"valve", (char *)"--command", NULL};
  char **cases[] = {inline_form, bare_form};

  for (size_t i = 0; i < 2; ++i) {
    valve_t *v = parser_for_(options, 1);
    EXPECT(v != NULL && vl_parse(v, 2, cases[i]) == -1,
           "named command form rejected");
    const vl_error_t *error = vl_error_at(v, 0);
    EXPECT(error && error->code == VL_ERROR_DISABLED_FORM &&
               strcmp(error->key, "command") == 0 &&
               strcmp(error->message, "command is given after --") == 0,
           "named command form explains the required syntax");
    vl_destroy(v);
  }
}

void test_command_schema_shape_is_validated(void) {
  const vl_option_t base = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  vl_option_t option = base;
  const vl_option_t *options[] = {&option, NULL};

  const vl_target_t valid_targets[] = {
      VL_TARGET_NONE,
      VL_TARGET_COMMAND,
      VL_TARGET_VALUE,
  };
  for (size_t i = 0; i < 3; ++i) {
    option = base;
    option.target = valid_targets[i];
    valve_t *v = parser_for_(options, 1);
    EXPECT(v != NULL, "valid command target accepted");
    vl_destroy(v);
  }

  option = base;
  option.type = VL_OPT_TYPE_LONG;
  EXPECT(parser_for_(options, 1) == NULL, "command long type rejected");
  option = base;
  option.short_name = 'c';
  EXPECT(parser_for_(options, 1) == NULL, "command short name rejected");
  option = base;
  option.toggle_ref = "command";
  EXPECT(parser_for_(options, 1) == NULL, "command toggle reference rejected");
  option = base;
  option.repeat = VL_OPTION_REPEAT_ARRAY;
  EXPECT(parser_for_(options, 1) == NULL, "command repeat array rejected");
  option = base;
  option.target = VL_TARGET_STRING;
  EXPECT(parser_for_(options, 1) == NULL, "command scalar target rejected");
}

void test_only_one_command_option_is_allowed_per_chain(void) {
  const vl_option_t first = {
      .name = "first",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  const vl_option_t second = {
      .name = "second",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  const vl_option_t *const both[] = {&first, &second, NULL};
  const vl_option_t *const first_only[] = {&first, NULL};
  const vl_option_t *const second_only[] = {&second, NULL};
  const vl_verb_t local = {.name = "run", .options = second_only};
  const vl_verb_t *const local_verbs[] = {&local, NULL};
  const vl_executable_t global_pair = {.options = both};
  const vl_executable_t global_and_verb = {
      .options = first_only,
      .verbs = local_verbs,
  };

  EXPECT(vl_create(&global_pair) == NULL, "two global commands rejected");
  EXPECT(vl_create(&global_and_verb) == NULL,
         "global and verb commands on one chain rejected");

  const vl_verb_t nested = {.name = "child", .options = second_only};
  const vl_verb_t *const nested_verbs[] = {&nested, NULL};
  const vl_verb_t parent = {
      .name = "parent",
      .options = first_only,
      .verbs = nested_verbs,
  };
  const vl_verb_t *const parent_verbs[] = {&parent, NULL};
  const vl_executable_t parent_and_child = {.verbs = parent_verbs};
  EXPECT(vl_create(&parent_and_child) == NULL,
         "verb and sub-verb commands on one chain rejected");

  const vl_verb_t sibling_a = {.name = "a", .options = first_only};
  const vl_verb_t sibling_b = {.name = "b", .options = second_only};
  const vl_verb_t *const siblings[] = {&sibling_a, &sibling_b, NULL};
  const vl_verb_t sibling_parent = {.name = "parent", .verbs = siblings};
  const vl_verb_t *const sibling_roots[] = {&sibling_parent, NULL};
  const vl_executable_t sibling_settings = {.verbs = sibling_roots};
  valve_t *v = vl_create(&sibling_settings);
  EXPECT(v != NULL, "sibling sub-verbs may each declare a command");
  vl_destroy(v);
}

void test_command_lookup_follows_active_scope(void) {
  vl_command_t target = {0};
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
      .data = &target,
      .target = VL_TARGET_COMMAND,
  };
  const vl_option_t *const commands[] = {&command, NULL};
  const vl_option_t child_flag = {
      .name = "child-flag",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  const vl_option_t *const child_options[] = {&child_flag, NULL};
  const vl_verb_t child = {.name = "child", .options = child_options};
  const vl_verb_t *const children[] = {&child, NULL};
  const vl_verb_t parent = {
      .name = "parent",
      .options = commands,
      .verbs = children,
  };
  const vl_verb_t *const verbs[] = {&parent, NULL};
  const vl_executable_t settings = {.verbs = verbs};
  char *argv[] = {(char *)"valve", (char *)"parent", (char *)"child",
                  (char *)"--", (char *)"tool", NULL};
  valve_t *v = vl_create(&settings);

  EXPECT(v != NULL && vl_parse(v, 5, argv) == 0,
         "sub-verb chain finds parent command option");
  EXPECT(target.argv == &argv[4] && target.argc == 1,
         "active parent command receives tail");
  vl_destroy(v);
  vl_targets_clear(&settings);

  vl_command_t local_target = {0};
  const vl_option_t local_command = {
      .name = "local-command",
      .value = VL_OPTION_VALUE_COMMAND,
      .data = &local_target,
      .target = VL_TARGET_COMMAND,
  };
  const vl_option_t *const local_commands[] = {&local_command, NULL};
  const vl_verb_t local_child = {
      .name = "child",
      .options = local_commands,
  };
  const vl_verb_t *const local_children[] = {&local_child, NULL};
  const vl_verb_t local_parent = {
      .name = "parent",
      .verbs = local_children,
  };
  const vl_verb_t *const local_verbs[] = {&local_parent, NULL};
  const vl_executable_t local_settings = {.verbs = local_verbs};
  v = vl_create(&local_settings);

  EXPECT(v != NULL && vl_parse(v, 5, argv) == 0,
         "sub-verb chain finds local command option");
  EXPECT(local_target.argv == &argv[4] && local_target.argc == 1,
         "active sub-verb command receives tail");
  vl_destroy(v);
  vl_targets_clear(&local_settings);
}

void test_command_target_rejects_non_command_value(void) {
  vl_command_t target = {0};
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
      .data = &target,
      .target = VL_TARGET_COMMAND,
  };
  const vl_option_t *const options[] = {&command, NULL};
  vl_value_t wrong = {.kind = VL_VALUE_BOOL, .as.boolean = true};
  valve_t *v = parser_for_(options, 1);

  EXPECT(v != NULL && vl_result_set_(v, &command, &wrong, 1) == 0,
         "command target rejects a non-command value");
  const vl_error_t *error = vl_error_at(v, 0);
  EXPECT(error && error->code == VL_ERROR_INVALID_VALUE &&
             strcmp(error->message, "target expects command value") == 0,
         "command target mismatch is structured");
  vl_destroy(v);
}

void test_command_result_allocation_failure_is_propagated(void) {
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  const vl_option_t *const options[] = {&command, NULL};
  char *argv[] = {(char *)"valve", (char *)"--", (char *)"tool", NULL};
  valve_t *v = parser_for_(options, 1);

  vl_test_alloc_fail_after(1);
  EXPECT(v != NULL && vl_parse(v, 3, argv) == -1,
         "command result allocation failure reaches the caller");
  vl_test_alloc_reset();
  vl_destroy(v);
}

void test_required_command_uses_existing_presence_check(void) {
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
      .required = true,
  };
  const vl_option_t *const commands[] = {&command, NULL};
  const vl_verb_t exec = {.name = "exec", .options = commands};
  const vl_verb_t *const verbs[] = {&exec, NULL};
  const vl_executable_t settings = {.verbs = verbs};
  char *argv[] = {(char *)"key", (char *)"exec", NULL};
  valve_t *v = vl_create(&settings);

  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1,
         "missing required command fails");
  const vl_error_t *error = vl_error_at(v, 0);
  EXPECT(error && error->code == VL_ERROR_MISSING_REQUIRED &&
             strcmp(error->key, "command") == 0,
         "required command uses existing structured error");
  vl_destroy(v);
}

void test_command_relationships_use_command_labels(void) {
  vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  vl_option_t flag = {
      .name = "flag",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  const vl_option_t *requires_command[] = {&command, NULL};
  const vl_option_t *conflicts_command[] = {&command, NULL};
  flag.requires = requires_command;
  const vl_option_t *const options[] = {&command, &flag, NULL};
  char *flag_only[] = {(char *)"valve", (char *)"--flag", NULL};
  valve_t *v = parser_for_(options, 2);

  EXPECT(v != NULL && vl_parse(v, 2, flag_only) == -1,
         "normal option may require command presence");
  const vl_error_t *error = vl_error_at(v, 0);
  EXPECT(error && error->code == VL_ERROR_UNSATISFIED_REQUIREMENT &&
             strcmp(error->message, "requires -- <command> [args…]") == 0,
         "requirement diagnostic names the command syntax");
  vl_destroy(v);

  flag.requires = NULL;
  flag.conflicts = conflicts_command;
  char *both[] = {(char *)"valve", (char *)"--flag", (char *)"--",
                  (char *)"tool", NULL};
  v = parser_for_(options, 2);
  EXPECT(v != NULL && vl_parse(v, 4, both) == -1,
         "command participates in conflict checks");
  error = vl_error_at(v, 0);
  EXPECT(error && error->code == VL_ERROR_CONFLICTING_OPTION,
         "command conflict is a structured conflict, not allocation failure");
  vl_destroy(v);
}

void test_command_help_is_last_and_targetable(void) {
  vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  vl_option_t flag = {
      .name = "flag",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  const vl_option_t *conflicts[] = {&command, NULL};
  flag.conflicts = conflicts;
  const vl_option_t *const options[] = {&command, &flag, NULL};
  char *overview[] = {(char *)"valve", (char *)"--help", NULL};
  char *targeted[] = {(char *)"valve", (char *)"--help=command", NULL};
  valve_t *v = parser_for_(options, 2);
  char *text = capture_parse_(v, 2, overview);

  EXPECT(text != NULL, "overview help captured");
  const char *flag_at = text ? strstr(text, "--flag") : NULL;
  const char *command_at = text ? strstr(text, "-- <command> [args…]") : NULL;
  EXPECT(flag_at && command_at && flag_at < command_at,
         "command option prints last despite declaration order");
  EXPECT(text && strstr(text, "conflicts: -- <command> [args…]"),
         "relationship annotation uses command syntax");
  free(text);
  vl_destroy(v);

  v = parser_for_(options, 2);
  text = capture_parse_(v, 2, targeted);
  EXPECT(text && strstr(text, "global option -- <command> [args…]"),
         "targeted help card uses command syntax");
  EXPECT(!text || !strstr(text, "global option --command"),
         "targeted help does not invent a named command form");
  free(text);
  vl_destroy(v);
}

void test_command_value_label_and_option_label(void) {
  const vl_option_t command = {
      .name = "command",
      .value = VL_OPTION_VALUE_COMMAND,
  };
  char *label = option_label_(&command);

  EXPECT(strcmp(option_value_label_(&command), "command") == 0,
         "command value label exposed");
  EXPECT(label && strcmp(label, "-- <command> [args…]") == 0,
         "command relationship label exposed");
  const vl_option_t *const sparse[] = {NULL};
  EXPECT(options_command_count_(sparse, 1) == 0,
         "command counter ignores a null table entry");
  free(label);
}
