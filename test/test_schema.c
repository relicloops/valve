#include "test_support.h"

/* === ceedling valve sources (auto) === */
TEST_SOURCE_FILE("action_count_.c")
TEST_SOURCE_FILE("actions_valid_.c")
TEST_SOURCE_FILE("annotations_print_.c")
TEST_SOURCE_FILE("array_free_.c")
TEST_SOURCE_FILE("at.c")
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
TEST_SOURCE_FILE("seen_.c")
TEST_SOURCE_FILE("set_.c")
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


void setUp(void) {}
void tearDown(void) {}

void test_schema_null_options_with_count_rejected(void) {
  const vl_executable_t settings = {
      .options = NULL,
      .option_count = 1,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "NULL options with nonzero count rejected");
  vl_destroy(v);
}

void test_schema_null_commands_with_count_rejected(void) {
  const vl_executable_t settings = {
      .verbs = NULL,
      .verb_count = 1,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "NULL verbs with nonzero count rejected");
  vl_destroy(v);
}

void test_schema_null_command_options_with_count_rejected(void) {
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = NULL, .option_count = 1),
      NULL,
  };
  const vl_executable_t settings = {
      .verbs = verbs,
      .verb_count = 1,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "NULL verb options with nonzero count rejected");
  vl_destroy(v);
}

void test_schema_invalid_assign_rejected(void) {
  const vl_executable_t settings = {
      .assign = (vl_assign_t)99,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "invalid assignment form rejected");
  vl_destroy(v);
}

void test_schema_empty_option_name_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "empty option name rejected");
  vl_destroy(v);
}

void test_schema_option_name_with_equals_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "bad=name", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "option name containing equals rejected");
  vl_destroy(v);
}

void test_schema_dash_option_name_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "-bad", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "option name starting with dash rejected");
  vl_destroy(v);
}

void test_schema_invalid_short_name_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "bad", .short_name = '-', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "invalid short option name rejected");
  vl_destroy(v);
}

void test_schema_invalid_option_enum_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "bad", .type = VL_OPT_TYPE_LONG,
             .value = (vl_option_value_t)99),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "invalid option value enum rejected");
  vl_destroy(v);
}

void test_schema_invalid_repeat_enum_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "bad", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .repeat = (vl_option_repeat_t)99),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "invalid repeat enum rejected");
  vl_destroy(v);
}

void test_schema_invalid_target_enum_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "bad", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .target = (vl_target_t)99),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "invalid target enum rejected");
  vl_destroy(v);
}

void test_schema_unknown_form_bits_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "bad", .type = (vl_opt_type_t)(1u << 20),
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 1};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "unknown form bits rejected");
  vl_destroy(v);
}

void test_schema_empty_command_name_rejected(void) {
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = ""),
      NULL,
  };
  const vl_executable_t settings = {.verbs = verbs};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "empty verb name rejected");
  vl_destroy(v);
}

void test_schema_dash_command_name_rejected(void) {
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "-run"),
      NULL,
  };
  const vl_executable_t settings = {.verbs = verbs};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "verb name starting with dash rejected");
  vl_destroy(v);
}

void test_schema_duplicate_long_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 2};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "duplicate long names rejected by vl_create");
  vl_destroy(v);
}

void test_schema_duplicate_short_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "alpha", .short_name = 'x', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "beta", .short_name = 'x', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_executable_t settings = {.options = options, .option_count = 2};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "duplicate short names rejected by vl_create");
  vl_destroy(v);
}

void test_schema_duplicate_command_rejected(void) {
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run"),
      VL_CMD(.name = "run"),
      NULL,
  };
  const vl_executable_t settings = {.verbs = verbs};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "duplicate verb names rejected by vl_create");
  vl_destroy(v);
}

void test_schema_command_collides_with_global_rejected(void) {
  const vl_option_t *const global_options[] = {
      VL_OPT(.name = "tags", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const local_options[] = {
      VL_OPT(.name = "tags", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "build", .options = local_options),
      NULL,
  };
  const vl_executable_t settings = {
      .options = global_options,
      .verbs = verbs,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "verb-local long name colliding with global rejected");
  vl_destroy(v);
}

void test_schema_command_short_collides_with_global_rejected(void) {
  const vl_option_t *const global_options[] = {
      VL_OPT(.name = "verbose", .short_name = 'v',
             .type = VL_OPT_TYPE_SHORT, .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const local_options[] = {
      VL_OPT(.name = "version", .short_name = 'v',
             .type = VL_OPT_TYPE_SHORT, .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "build", .options = local_options),
      NULL,
  };
  const vl_executable_t settings = {
      .options = global_options,
      .verbs = verbs,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "verb-local short name colliding with global rejected");
  vl_destroy(v);
}

void test_schema_reserved_option_name_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "help", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  vl_executable_t settings = {.options = options};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "option named 'help' rejected in strict mode");
  vl_destroy(v);
}

void test_schema_reserved_command_name_rejected(void) {
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "version"),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "verb named 'version' rejected in strict mode");
  vl_destroy(v);
}

void test_schema_reserved_short_name_rejected(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "host", .short_name = 'h', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  vl_executable_t settings = {.options = options};
  valve_t *v = vl_create(&settings);
  EXPECT(v == NULL, "option with short_name 'h' rejected in strict mode");
  vl_destroy(v);
}

