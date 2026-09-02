#include "test_support.h"

/* === ceedling valve sources (auto) === */
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

void test_long_equals_number(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.lanes", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_AUTO, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--proxy.lanes=8",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for long equals number");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse --flag=value");
  value = vl_get(v, "proxy.lanes");
  EXPECT(value && value->kind == VL_VALUE_INT && value->as.integer == 8,
         "--flag=value coerces integer");
  vl_destroy(v);
}

void test_short_equals_number(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "ratio", .short_name = 'r', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_AUTO, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"-r=1.5",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for short equals number");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse -f=value");
  value = vl_get(v, "ratio");
  EXPECT(value && value->kind == VL_VALUE_DOUBLE && value->as.number > 1.49 &&
             value->as.number < 1.51,
         "-f=value coerces double");
  vl_destroy(v);
}

void test_next_values_and_strings(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_AUTO, ),
      VL_OPT(.name = "output", .short_name = 'o', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_AUTO, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve", (char *)"--mode", (char *)"true",
      (char *)"-o",    (char *)"false",
  };
  valve_t *v = parser_with_form_(options, 2, VL_ASSIGN_SEPARATE);
  const vl_value_t *mode = NULL;
  const vl_value_t *output = NULL;

  EXPECT(v != NULL, "parser for next values");
  EXPECT(vl_parse(v, 5, argv) == 0, "parse --flag value and -f value");
  mode = vl_get(v, "mode");
  output = vl_get(v, "output");
  EXPECT(mode && mode->kind == VL_VALUE_BOOL && mode->as.boolean,
         "true auto-types bool");
  EXPECT(output && output->kind == VL_VALUE_BOOL && !output->as.boolean,
         "false auto-types bool");
  vl_destroy(v);
}

void test_enable_disable_boolean(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "enable-feature", .toggle_ref = "feature",
             .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--disable-feature",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for enable disable");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse --disable");
  value = vl_get(v, "enable-feature");
  EXPECT(value && value->kind == VL_VALUE_BOOL && value->as.boolean == false,
         "--disable-feature stores false");
  vl_destroy(v);
}

void test_disabled_form_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "name", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_AUTO, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--name",
      (char *)"value",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for disabled form");
  EXPECT(vl_parse(v, 3, argv) == -1, "disabled form fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_DISABLED_FORM &&
             strcmp(err->key, "name") == 0,
         "disabled form records structured error");
  vl_destroy(v);
}

void test_unknown_option_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "known", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_AUTO, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--unknown=1",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for unknown option");
  EXPECT(vl_parse(v, 2, argv) == -1, "unknown option fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNKNOWN_OPTION &&
             strcmp(err->key, "unknown") == 0,
         "unknown option records structured error");
  vl_destroy(v);
}

void test_duplicate_option_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--mode=accident",
      (char *)"--mode=hello",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for duplicate option error");
  EXPECT(vl_parse(v, 3, argv) == -1, "duplicate option fails by default");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_DUPLICATE_OPTION &&
             strcmp(err->key, "mode") == 0,
         "duplicate option records structured error");
  vl_destroy(v);
}

void test_duplicate_option_array(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING,
             .repeat = VL_OPTION_REPEAT_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--mode=accident",
      (char *)"--mode=hello",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for duplicate option array");
  EXPECT(vl_parse(v, 3, argv) == 0, "duplicate option can become array");
  value = vl_get(v, "mode");
  EXPECT(value && value->kind == VL_VALUE_ARRAY && value->as.array.count == 2,
         "duplicate values are stored as array");
  EXPECT(value && strcmp(value->as.array.items[0].raw, "accident") == 0 &&
             strcmp(value->as.array.items[1].raw, "hello") == 0,
         "array preserves duplicate values");
  vl_destroy(v);
}

void test_dotted_schema_and_int_max(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT, .has_int_max = true, .int_max = 4, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--proxy.lane=api.xxx.loop",
      (char *)"--proxy.cpus=2",
  };
  valve_t *v = parser_(options, 2);
  const vl_value_t *lane = NULL;
  const vl_value_t *cpus = NULL;

  EXPECT(v != NULL, "parser for dotted schema");
  EXPECT(vl_parse(v, 3, argv) == 0, "parse dotted schema keys");
  lane = vl_get(v, "proxy.lane");
  cpus = vl_get(v, "proxy.cpus");
  EXPECT(lane && lane->kind == VL_VALUE_STRING &&
             strcmp(lane->raw, "api.xxx.loop") == 0,
         "dotted lane is typed string");
  EXPECT(cpus && cpus->kind == VL_VALUE_INT && cpus->as.integer == 2,
         "dotted cpus is typed int");
  vl_destroy(v);
}

void test_int_max_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT, .has_int_max = true, .int_max = 4, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--proxy.cpus=8",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for int max error");
  EXPECT(vl_parse(v, 2, argv) == -1, "int above max fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE &&
             strcmp(err->key, "proxy.cpus") == 0,
         "int max records structured error");
  vl_destroy(v);
}

void test_null_terminated_options(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  const vl_executable_t settings = {
      .options = options,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--proxy.lane=api.xxx.loop",
  };
  valve_t *v = vl_create(&settings);
  const vl_value_t *lane = NULL;

  EXPECT(v != NULL, "parser for null-terminated options");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse null-terminated option schema");
  lane = vl_get(v, "proxy.lane");
  EXPECT(lane && lane->kind == VL_VALUE_STRING &&
             strcmp(lane->raw, "api.xxx.loop") == 0,
         "null-terminated schema stores result");
  vl_destroy(v);
}

void test_enable_disable_both_assignment_modes(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "enable-feature", .toggle_ref = "feature",
             .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--enable-feature",
  };
  valve_t *equals = parser_(options, 1);
  valve_t *next = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);

  EXPECT(equals != NULL && next != NULL, "parsers for assignment toggles");
  EXPECT(vl_parse(equals, 2, argv) == 0, "enable parses in equals mode");
  EXPECT(vl_parse(next, 2, argv) == 0, "enable parses in next mode");
  EXPECT(vl_get(equals, "enable-feature") &&
             vl_get(equals, "enable-feature")->as.boolean == true,
         "equals mode stores enabled toggle");
  EXPECT(vl_get(next, "enable-feature") &&
             vl_get(next, "enable-feature")->as.boolean == true,
         "next mode stores enabled toggle");
  vl_destroy(equals);
  vl_destroy(next);
}

