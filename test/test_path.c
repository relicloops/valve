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

/* ---- vl_path_split --------------------------------------------------- */

void test_path_single_segment(void) {
  char scratch[32];
  vl_path_t path = {0};
  size_t n = vl_path_split("list", scratch, sizeof(scratch), &path);
  EXPECT(n == 1, "single segment returns count 1");
  EXPECT(path.count == 1, "single segment path.count is 1");
  EXPECT(path.segments[0] && strcmp(path.segments[0], "list") == 0,
         "single segment value is 'list'");
}

void test_path_two_segments(void) {
  char scratch[32];
  vl_path_t path = {0};
  size_t n = vl_path_split("proxy.lane", scratch, sizeof(scratch), &path);
  EXPECT(n == 2, "two segments returns count 2");
  EXPECT(path.segments[0] && strcmp(path.segments[0], "proxy") == 0,
         "segment 0 is 'proxy'");
  EXPECT(path.segments[1] && strcmp(path.segments[1], "lane") == 0,
         "segment 1 is 'lane'");
}

void test_path_three_segments(void) {
  char scratch[64];
  vl_path_t path = {0};
  size_t n =
      vl_path_split("agent.call.request", scratch, sizeof(scratch), &path);
  EXPECT(n == 3, "three segments returns count 3");
  EXPECT(path.segments[2] && strcmp(path.segments[2], "request") == 0,
         "segment 2 is 'request'");
}

void test_path_max_segments(void) {
  char scratch[32];
  vl_path_t path = {0};
  size_t n = vl_path_split("a.b.c.d", scratch, sizeof(scratch), &path);
  EXPECT(n == 4, "four segments (max) returns count 4");
  EXPECT(path.segments[3] && strcmp(path.segments[3], "d") == 0,
         "segment 3 is 'd'");
}

void test_path_overflow(void) {
  char scratch[32];
  vl_path_t path = {0};
  size_t n = vl_path_split("a.b.c.d.e", scratch, sizeof(scratch), &path);
  EXPECT(n == 0, "five segments overflow returns 0");
  EXPECT(path.count == 0, "overflow leaves path.count 0");
}

void test_path_empty(void) {
  char scratch[8];
  vl_path_t path = {0};
  EXPECT(vl_path_split("", scratch, sizeof(scratch), &path) == 0,
         "empty input returns 0");
}

void test_path_empty_segments(void) {
  char scratch[16];
  vl_path_t path = {0};
  EXPECT(vl_path_split(".a", scratch, sizeof(scratch), &path) == 0,
         "leading dot returns 0");
  EXPECT(vl_path_split("a.", scratch, sizeof(scratch), &path) == 0,
         "trailing dot returns 0");
  EXPECT(vl_path_split("a..b", scratch, sizeof(scratch), &path) == 0,
         "double dot returns 0");
}

void test_path_scratch_too_small(void) {
  char scratch[4];
  vl_path_t path = {0};
  /* "proxy.lane" needs 11 bytes incl. NUL; scratch holds 4. */
  EXPECT(vl_path_split("proxy.lane", scratch, sizeof(scratch), &path) == 0,
         "scratch too small returns 0");
}

void test_path_null_args(void) {
  char scratch[8];
  vl_path_t path = {0};
  EXPECT(vl_path_split(NULL, scratch, sizeof(scratch), &path) == 0,
         "NULL dotted returns 0");
  EXPECT(vl_path_split("a", NULL, sizeof(scratch), &path) == 0,
         "NULL scratch returns 0");
  EXPECT(vl_path_split("a", scratch, sizeof(scratch), NULL) == 0,
         "NULL out returns 0");
  EXPECT(vl_path_split("a", scratch, 0, &path) == 0,
         "zero scratch_len returns 0");
}

/* ---- VL_OPTION_VALUE_DOT_NOTATION scalar coexistence ----------------- */

typedef struct dot_proxy {
  char *lane;
  int cpus;
} dot_proxy_t;

void test_dot_notation_string_and_int(void) {
  dot_proxy_t proxy = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .data = &proxy,
             .offset = offsetof(dot_proxy_t, lane), .target = VL_TARGET_STRING),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .data = &proxy,
             .offset = offsetof(dot_proxy_t, cpus), .target = VL_TARGET_INT,
             .has_int_min = true, .has_int_max = true, .int_min = 1,
             .int_max = 8),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--proxy.lane=api.loop",
      (char *)"--proxy.cpus=4",
  };
  valve_t *v = parser_(options, 2);
  const vl_value_t *lane = NULL;
  const vl_value_t *cpus = NULL;

  EXPECT(v != NULL, "dot-notation parser created");
  EXPECT(vl_parse(v, 3, argv) == 0, "dot-notation options parse without error");

  lane = vl_get(v, "proxy.lane");
  EXPECT(lane && lane->kind == VL_VALUE_STRING &&
             strcmp(lane->raw, "api.loop") == 0,
         "dot-notation string leaf stored as string");

  cpus = vl_get(v, "proxy.cpus");
  EXPECT(cpus && cpus->kind == VL_VALUE_INT && cpus->as.integer == 4,
         "dot-notation int leaf stored as int (target-driven)");

  vl_destroy(v);
  EXPECT(proxy.lane && strcmp(proxy.lane, "api.loop") == 0,
         "dot-notation string bound to caller target");
  EXPECT(proxy.cpus == 4, "dot-notation int bound to caller target");
  free(proxy.lane);
}

void test_dot_notation_int_bounds(void) {
  dot_proxy_t proxy = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .data = &proxy,
             .offset = offsetof(dot_proxy_t, cpus), .target = VL_TARGET_INT,
             .has_int_max = true, .int_max = 4),
      NULL,
  };
  char *argv[] = {(char *)"valve", (char *)"--proxy.cpus=99"};
  valve_t *v = parser_(options, 1);

  EXPECT(v != NULL, "dot-notation bounds parser created");
  EXPECT(vl_parse(v, 2, argv) == -1, "dot-notation int honors target bounds");
  vl_destroy(v);
}

void test_dot_notation_int_rejects_nonint(void) {
  dot_proxy_t proxy = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .data = &proxy,
             .offset = offsetof(dot_proxy_t, cpus), .target = VL_TARGET_INT),
      NULL,
  };
  char *argv[] = {(char *)"valve", (char *)"--proxy.cpus=abc"};
  valve_t *v = parser_(options, 1);

  EXPECT(v != NULL, "dot-notation non-int parser created");
  EXPECT(vl_parse(v, 2, argv) == -1, "dot-notation int rejects non-integer");
  vl_destroy(v);
}

