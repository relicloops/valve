#include "test_support.h"

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


void setUp(void) {}
void tearDown(void) {}

/* Parses `--wait=<literal>` and reports the stored seconds, or -1 when the
   parse failed. Every accept-case test is the same shape, so the shape lives
   here instead of in each of them. */
static int64_t duration_seconds_(const char *literal) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME, ),
      NULL,
  };
  char arg[64];
  snprintf(arg, sizeof arg, "--wait=%s", literal);
  char *argv[] = {(char *)"valve", arg};
  valve_t *v = parser_(options, 1);
  int64_t seconds = -1;

  if (!v)
    return -1;

  if (vl_parse(v, 2, argv) == 0) {
    const vl_value_t *value = vl_get(v, "wait");
    if (value && value->kind == VL_VALUE_INT)
      seconds = value->as.integer;
  }

  vl_destroy(v);
  return seconds;
}

/* Returns the first error code recorded for `--wait=<literal>`, or 0 when the
   literal unexpectedly parsed. */
static int duration_error_code_(const char *literal) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME, ),
      NULL,
  };
  char arg[64];
  snprintf(arg, sizeof arg, "--wait=%s", literal);
  char *argv[] = {(char *)"valve", arg};
  valve_t *v = parser_(options, 1);
  int code = 0;

  if (!v)
    return -1;

  if (vl_parse(v, 2, argv) != 0) {
    const vl_error_t *err = vl_error_at(v, 0);
    if (err)
      code = (int)err->code;
  }

  vl_destroy(v);
  return code;
}

void test_duration_bare_integer_is_seconds(void) {
  EXPECT(duration_seconds_("30") == 30,
         "bare integer duration means seconds");
  EXPECT(duration_seconds_("0") == 0, "zero duration is accepted");
}

void test_duration_single_units(void) {
  EXPECT(duration_seconds_("45s") == 45, "s suffix is seconds");
  EXPECT(duration_seconds_("5m") == 300, "m suffix is minutes");
  EXPECT(duration_seconds_("2h") == 7200, "h suffix is hours");
  EXPECT(duration_seconds_("7d") == 604800, "d suffix is days");
}

void test_duration_compound(void) {
  EXPECT(duration_seconds_("1h30m") == 5400, "compound h+m duration sums");
  EXPECT(duration_seconds_("2h30m") == 9000, "compound 2h30m duration sums");
  EXPECT(duration_seconds_("1d2h3m4s") == 93784,
         "compound d+h+m+s duration sums");
}

void test_duration_rejects_bad_literals(void) {
  EXPECT(duration_error_code_("") == VL_ERROR_INVALID_VALUE,
         "empty duration is rejected");
  EXPECT(duration_error_code_("-5") == VL_ERROR_INVALID_VALUE,
         "negative duration is rejected");
  EXPECT(duration_error_code_("10x") == VL_ERROR_INVALID_VALUE,
         "unknown duration suffix is rejected");
  EXPECT(duration_error_code_("abc") == VL_ERROR_INVALID_VALUE,
         "non-numeric duration is rejected");
  EXPECT(duration_error_code_("1h30") == VL_ERROR_INVALID_VALUE,
         "trailing suffix-less segment is rejected, not guessed at");
  EXPECT(duration_error_code_("5m3") == VL_ERROR_INVALID_VALUE,
         "suffix-less segment after a unit is rejected");
  EXPECT(duration_error_code_("99999999999999999999d") ==
             VL_ERROR_INVALID_VALUE,
         "overflowing duration is rejected");
}

void test_duration_error_names_the_token(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME, ),
      NULL,
  };
  char *argv[] = {(char *)"valve", (char *)"--wait=10x"};
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for duration error message");
  EXPECT(vl_parse(v, 2, argv) == -1, "bad duration fails the parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->message && strstr(err->message, "10x") != NULL,
         "duration error message names the bad token");
  EXPECT(err && err->key && strcmp(err->key, "wait") == 0,
         "duration error is attributed to its option");
  vl_destroy(v);
}

void test_duration_bounds_apply_to_seconds(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME, .has_int_min = true,
             .int_min = 5, .has_int_max = true, .int_max = 3600, ),
      NULL,
  };
  /* 2s is below the 5s floor even though "2" alone would be a fine integer,
     and 2h is above the 3600s ceiling even though "2" would be well inside
     it -- the bounds are checked after conversion, not on the literal. */
  char *below[] = {(char *)"valve", (char *)"--wait=2s"};
  char *above[] = {(char *)"valve", (char *)"--wait=2h"};
  char *inside[] = {(char *)"valve", (char *)"--wait=5m"};

  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;
  EXPECT(v != NULL, "parser for duration below minimum");
  EXPECT(vl_parse(v, 2, below) == -1, "duration below minimum is rejected");
  err = vl_error_at(v, 0);
  EXPECT(err && err->message &&
             strcmp(err->message, "duration value is below minimum") == 0,
         "below-minimum message says duration, not integer");
  vl_destroy(v);

  v = parser_(options, 1);
  EXPECT(v != NULL, "parser for duration above maximum");
  EXPECT(vl_parse(v, 2, above) == -1, "duration above maximum is rejected");
  err = vl_error_at(v, 0);
  EXPECT(err && err->message &&
             strcmp(err->message, "duration value is above maximum") == 0,
         "above-maximum message says duration, not integer");
  vl_destroy(v);

  v = parser_(options, 1);
  EXPECT(v != NULL, "parser for duration inside bounds");
  EXPECT(vl_parse(v, 2, inside) == 0, "duration inside bounds is accepted");
  const vl_value_t *value = vl_get(v, "wait");
  EXPECT(value && value->kind == VL_VALUE_INT && value->as.integer == 300,
         "bounded duration stores converted seconds");
  vl_destroy(v);
}

void test_duration_writes_int_targets(void) {
  int seconds_int = 0;
  int64_t seconds_int64 = 0;
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME, .data = &seconds_int,
             .target = VL_TARGET_INT, ),
      VL_OPT(.name = "linger", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME, .data = &seconds_int64,
             .target = VL_TARGET_INT64, ),
      NULL,
  };
  char *argv[] = {(char *)"valve", (char *)"--wait=1h30m",
                  (char *)"--linger=2d"};
  valve_t *v = parser_(options, 2);

  EXPECT(v != NULL, "parser for duration targets");
  EXPECT(vl_parse(v, 3, argv) == 0, "duration options populate targets");
  vl_destroy(v);

  EXPECT(seconds_int == 5400, "duration writes seconds into an int target");
  EXPECT(seconds_int64 == 172800,
         "duration writes seconds into an int64 target");
}

void test_duration_separate_assign_form(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME, ),
      NULL,
  };
  char *argv[] = {(char *)"valve", (char *)"--wait", (char *)"5m"};
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);

  EXPECT(v != NULL, "parser for separate-form duration");
  EXPECT(vl_parse(v, 3, argv) == 0, "duration parses in separate assign form");
  const vl_value_t *value = vl_get(v, "wait");
  EXPECT(value && value->kind == VL_VALUE_INT && value->as.integer == 300,
         "separate-form duration stores converted seconds");
  vl_destroy(v);
}

