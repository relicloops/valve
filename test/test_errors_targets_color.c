#include "test_support.h"

/* === ceedling valve sources (auto) === */
TEST_SOURCE_FILE("annotations_print_.c")
TEST_SOURCE_FILE("array_free_.c")
TEST_SOURCE_FILE("at.c")
TEST_SOURCE_FILE("clear.c")
TEST_SOURCE_FILE("collides_with_globals_.c")
TEST_SOURCE_FILE("color.c")
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
TEST_SOURCE_FILE("options_copy_.c")
TEST_SOURCE_FILE("options_have_invalid_.c")
TEST_SOURCE_FILE("options_use_reserved_.c")
TEST_SOURCE_FILE("other_.c")
TEST_SOURCE_FILE("parse.c")
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


#include <stdio.h>

void setUp(void) {}
void tearDown(void) {
  vl_color_support_reset();
}

/* ---- errors foreach / print ------------------------------------------ */

typedef struct error_visit {
  size_t count;
  int argv_index;
  const char *key;
  vl_error_code_t code;
} error_visit_t;

static void visit_error_(const vl_error_t *err, void *userdata) {
  error_visit_t *visit = userdata;

  if (visit->count == 0) {
    visit->argv_index = err->argv_index;
    visit->key = err->key;
    visit->code = err->code;
  }
  ++visit->count;
}

void test_errors_foreach_visits_each(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "known", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--unknown=1",
      (char *)"--also-unknown=2",
  };
  valve_t *v = parser_(options, 1);
  error_visit_t visit = {0};

  EXPECT(v != NULL, "parser for errors foreach");
  EXPECT(vl_parse(v, 3, argv) == -1, "invalid argv fails parse");
  EXPECT(vl_error_count(v) >= 1, "parse recorded at least one error");

  vl_errors_foreach(v, visit_error_, &visit);
  EXPECT(visit.count == vl_error_count(v),
         "foreach visits every recorded error");
  EXPECT(visit.count >= 1 && visit.key && strcmp(visit.key, "unknown") == 0,
         "foreach sees first unknown option key");
  EXPECT(visit.code == VL_ERROR_UNKNOWN_OPTION,
         "foreach sees unknown-option code");

  vl_destroy(v);
}

void test_errors_print_format(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "known", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--unknown=1",
  };
  valve_t *v = parser_(options, 1);
  char *buf = NULL;
  size_t buflen = 0;
  FILE *mem = NULL;
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for errors print");
  EXPECT(vl_parse(v, 2, argv) == -1, "invalid argv fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err != NULL, "first error is present");

  mem = open_memstream(&buf, &buflen);
  EXPECT(mem != NULL, "open_memstream for errors print");
  vl_errors_print(v, mem);
  fflush(mem);
  fclose(mem);

  EXPECT(buf != NULL && strstr(buf, "error argv[") != NULL,
         "print starts with error argv[");
  EXPECT(buf && strstr(buf, "[unknown]") != NULL,
         "print includes [key]");
  EXPECT(buf && err && err->message && strstr(buf, err->message) != NULL,
         "print includes message");
  EXPECT(buf && strchr(buf, '\n') != NULL, "print ends lines with newline");

  free(buf);
  vl_destroy(v);
}

/* ---- targets clear --------------------------------------------------- */

typedef struct clear_cfg {
  char *lane;
  vl_value_t mode;
} clear_cfg_t;

void test_targets_clear_string_and_value(void) {
  clear_cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .data = &cfg,
             .offset = offsetof(clear_cfg_t, lane),
             .target = VL_TARGET_STRING, ),
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .data = &cfg,
             .offset = offsetof(clear_cfg_t, mode),
             .target = VL_TARGET_VALUE, ),
      NULL,
  };
  const vl_executable_t settings = {
      .options = options,
      .option_count = 2,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--lane=production",
      (char *)"--mode=dual",
  };
  valve_t *v = vl_create(&settings);

  EXPECT(v != NULL, "parser for targets clear");
  EXPECT(vl_parse(v, 3, argv) == 0, "parse populates string and value targets");
  EXPECT(cfg.lane != NULL && strcmp(cfg.lane, "production") == 0,
         "string target populated");
  EXPECT(cfg.mode.kind == VL_VALUE_STRING && cfg.mode.raw &&
             strcmp(cfg.mode.raw, "dual") == 0,
         "value target populated");
  vl_destroy(v);

  vl_targets_clear(&settings);
  EXPECT(cfg.lane == NULL, "string target cleared to NULL");
  EXPECT(cfg.mode.raw == NULL && cfg.mode.kind == VL_VALUE_STRING,
         "value target cleared");

  vl_targets_clear(&settings);
  EXPECT(cfg.lane == NULL, "second clear remains safe");
}

/* ---- color force / reset --------------------------------------------- */

void test_color_support_force_hierarchy(void) {
  vl_color_support_force(VL_COLOR_SUPPORT_NONE);
  EXPECT(!vl_color_supports(), "NONE: supports is false");
  EXPECT(!vl_color_supports_256(), "NONE: 256 is false");
  EXPECT(!vl_color_supports_truecolor(), "NONE: truecolor is false");

  vl_color_support_force(VL_COLOR_SUPPORT_BASIC);
  EXPECT(vl_color_supports(), "BASIC: supports is true");
  EXPECT(!vl_color_supports_256(), "BASIC: 256 is false");
  EXPECT(!vl_color_supports_truecolor(), "BASIC: truecolor is false");

  vl_color_support_force(VL_COLOR_SUPPORT_256);
  EXPECT(vl_color_supports(), "256: supports is true");
  EXPECT(vl_color_supports_256(), "256: 256 is true");
  EXPECT(!vl_color_supports_truecolor(), "256: truecolor is false");

  vl_color_support_force(VL_COLOR_SUPPORT_TRUECOLOR);
  EXPECT(vl_color_supports(), "TRUECOLOR: supports is true");
  EXPECT(vl_color_supports_256(), "TRUECOLOR: 256 is true");
  EXPECT(vl_color_supports_truecolor(), "TRUECOLOR: truecolor is true");

  vl_color_support_reset();
  EXPECT(vl_color_support_detect() == vl_color_support_detect(),
         "reset leaves detect callable");
}
