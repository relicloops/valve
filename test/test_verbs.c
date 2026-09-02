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

static valve_t *command_parser_(vl_assign_t assign) {
  const vl_option_t *const global_options[] = {
      &(const vl_option_t){
          .name = "tags",
          .type = VL_OPT_TYPE_LONG,
          .value = VL_OPTION_VALUE_ARRAY,
      },
      NULL,
  };
  const vl_option_t *const network_options[] = {
      &(const vl_option_t){
          .name = "proxy.lane",
          .type = VL_OPT_TYPE_LONG,
          .value = VL_OPTION_VALUE_STRING,
      },
      &(const vl_option_t){
          .name = "proxy.cpus",
          .type = VL_OPT_TYPE_LONG,
          .value = VL_OPTION_VALUE_INT,
      },
      NULL,
  };
  const vl_option_t *const config_options[] = {
      &(const vl_option_t){
          .name = "mode",
          .type = VL_OPT_TYPE_LONG,
          .value = VL_OPTION_VALUE_STRING,
      },
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      &(const vl_verb_t){
          .name = "network",
          .options = network_options,
      },
      &(const vl_verb_t){
          .name = "config",
          .options = config_options,
      },
      NULL,
  };
  const vl_executable_t settings = {
      .options = global_options,
      .verbs = verbs,
      .assign = assign,
  };

  return vl_create(&settings);
}

void test_command_network_success(void) {
  char *argv[] = {
      (char *)"valve",
      (char *)"network",
      (char *)"--proxy.lane=api/violation",
      (char *)"--proxy.cpus=2",
      (char *)"--tags=main",
  };
  valve_t *v = command_parser_(VL_ASSIGN_INLINE);
  const vl_value_t *lane = NULL;
  const vl_value_t *cpus = NULL;
  const vl_value_t *tags = NULL;

  EXPECT(v != NULL, "parser for network verb");
  EXPECT(vl_parse(v, 5, argv) == 0, "parse network verb options");
  EXPECT(vl_verb_get(v) && strcmp(vl_verb_get(v), "network") == 0,
         "vl_verb_get returns network");
  lane = vl_get(v, "proxy.lane");
  cpus = vl_get(v, "proxy.cpus");
  tags = vl_get(v, "tags");
  EXPECT(lane && strcmp(lane->raw, "api/violation") == 0,
         "network verb stores proxy lane");
  EXPECT(cpus && cpus->kind == VL_VALUE_INT && cpus->as.integer == 2,
         "network verb stores proxy cpus");
  EXPECT(tags && tags->kind == VL_VALUE_ARRAY && tags->as.array.count == 1 &&
             strcmp(tags->as.array.items[0].raw, "main") == 0,
         "global tags parse in network domain");
  vl_destroy(v);
}

void test_command_config_success(void) {
  char *argv[] = {
      (char *)"valve",
      (char *)"config",
      (char *)"--mode=dual",
      (char *)"--tags=internal",
  };
  valve_t *v = command_parser_(VL_ASSIGN_INLINE);
  const vl_value_t *mode = NULL;
  const vl_value_t *tags = NULL;

  EXPECT(v != NULL, "parser for config verb");
  EXPECT(vl_parse(v, 4, argv) == 0, "parse config verb options");
  EXPECT(vl_verb_get(v) && strcmp(vl_verb_get(v), "config") == 0,
         "vl_verb_get returns config");
  mode = vl_get(v, "mode");
  tags = vl_get(v, "tags");
  EXPECT(mode && strcmp(mode->raw, "dual") == 0, "config verb stores mode");
  EXPECT(tags && tags->kind == VL_VALUE_ARRAY && tags->as.array.count == 1 &&
             strcmp(tags->as.array.items[0].raw, "internal") == 0,
         "global tags parse in config domain");
  vl_destroy(v);
}

void test_command_local_option_isolated(void) {
  char *argv[] = {
      (char *)"valve",
      (char *)"config",
      (char *)"--proxy.lane=x",
  };
  valve_t *v = command_parser_(VL_ASSIGN_INLINE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for verb option isolation");
  EXPECT(vl_parse(v, 3, argv) == -1,
         "other verb's option fails in config domain");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNKNOWN_OPTION &&
             strcmp(err->key, "proxy.lane") == 0,
         "isolated verb option records unknown option");
  vl_destroy(v);
}

void test_missing_command_error(void) {
  char *argv[] = {
      (char *)"valve",
      (char *)"--tags=main",
  };
  valve_t *v = command_parser_(VL_ASSIGN_INLINE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for missing verb");
  EXPECT(vl_parse(v, 2, argv) == -1, "option in argv[1] is missing verb");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_MISSING_COMMAND,
         "missing verb records structured error");
  EXPECT(vl_verb_get(v) == NULL, "missing verb leaves no active verb");
  vl_destroy(v);
}

void test_unknown_command_error(void) {
  char *argv[] = {
      (char *)"valve",
      (char *)"unknown",
  };
  valve_t *v = command_parser_(VL_ASSIGN_INLINE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for unknown verb");
  EXPECT(vl_parse(v, 2, argv) == -1, "unknown argv[1] fails");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNKNOWN_COMMAND &&
             strcmp(err->key, "unknown") == 0,
         "unknown verb records structured error");
  EXPECT(vl_verb_get(v) == NULL, "unknown verb leaves no active verb");
  vl_destroy(v);
}

void test_single_command_without_commands(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "tags", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--tags=main",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *tags = NULL;

  EXPECT(v != NULL, "parser for single-verb mode");
  EXPECT(vl_parse(v, 2, argv) == 0,
         "single-verb parser still starts at argv[1]");
  EXPECT(vl_verb_get(v) == NULL, "single-verb parser has no verb");
  tags = vl_get(v, "tags");
  EXPECT(tags && tags->kind == VL_VALUE_ARRAY && tags->as.array.count == 1 &&
             strcmp(tags->as.array.items[0].raw, "main") == 0,
         "single-verb parser stores tags");
  vl_destroy(v);
}

void test_assignment_equals_rejects_next(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--mode",
      (char *)"dual",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for equals-only assignment");
  EXPECT(vl_parse(v, 3, argv) == -1, "equals mode rejects --flag value");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_DISABLED_FORM &&
             strcmp(err->key, "mode") == 0,
         "equals mode records disabled form");
  vl_destroy(v);
}

void test_assignment_next_rejects_equals(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--mode=dual",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for next-only assignment");
  EXPECT(vl_parse(v, 2, argv) == -1, "next mode rejects --flag=value");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_DISABLED_FORM &&
             strcmp(err->key, "mode") == 0,
         "next mode records disabled form");
  vl_destroy(v);
}

