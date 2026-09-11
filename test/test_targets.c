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

typedef struct target_proxy {
  char *lane;
  int cpus;
} target_proxy_t;

typedef struct target_config {
  target_proxy_t proxy;
  bool feature;
  vl_value_t mode;
} target_config_t;

void test_targets_survive_parser_destroy(void) {
  target_config_t config = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .data = &config.proxy,
             .offset = offsetof(target_proxy_t, lane),
             .target = VL_TARGET_STRING, ),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT, .data = &config.proxy,
             .offset = offsetof(target_proxy_t, cpus), .target = VL_TARGET_INT,
             .has_int_max = true, .int_max = 4, ),
      VL_OPT(.name = "enable-feature", .toggle_ref = "feature",
             .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, .data = &config,
             .offset = offsetof(target_config_t, feature),
             .target = VL_TARGET_BOOL, ),
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .repeat = VL_OPTION_REPEAT_ARRAY,
             .data = &config, .offset = offsetof(target_config_t, mode),
             .target = VL_TARGET_VALUE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",           (char *)"--proxy.lane=api.xxx.loop",
      (char *)"--proxy.cpus=2",  (char *)"--enable-feature",
      (char *)"--mode=accident", (char *)"--mode=hello",
  };
  valve_t *v = parser_(options, 4);

  EXPECT(v != NULL, "parser for caller targets");
  EXPECT(vl_parse(v, 6, argv) == 0, "parse and populate caller targets");
  vl_destroy(v);

  EXPECT(config.proxy.lane && strcmp(config.proxy.lane, "api.xxx.loop") == 0,
         "target string survives vl_destroy");
  EXPECT(config.proxy.cpus == 2, "target int survives vl_destroy");
  EXPECT(config.feature == true, "target bool survives vl_destroy");
  EXPECT(config.mode.kind == VL_VALUE_ARRAY && config.mode.as.array.count == 2,
         "target value array survives vl_destroy");
  EXPECT(strcmp(config.mode.as.array.items[0].raw, "accident") == 0 &&
             strcmp(config.mode.as.array.items[1].raw, "hello") == 0,
         "target value array keeps strings");

  free(config.proxy.lane);
  vl_value_clear(&config.mode);
}

void test_target_survives_partial_parse_failure(void) {
  char *lane = NULL;
  const vl_option_t *const options[] = {
      VL_OPT(.name = "lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .data = &lane,
             .target = VL_TARGET_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--lane=production",
      (char *)"--unknown=1",
  };
  valve_t *v = parser_(options, 1);

  EXPECT(v != NULL, "parser for partial parse target");
  EXPECT(vl_parse(v, 3, argv) == -1, "unknown option causes parse failure");
  EXPECT(lane != NULL && strcmp(lane, "production") == 0,
         "successfully populated target survives parse failure");
  vl_destroy(v);
  free(lane);
}

