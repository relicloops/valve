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

void test_long_next_rejects_option_lookalike(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "tags", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--tags",
      (char *)"--mode=foo",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for lookalike rejection");
  EXPECT(vl_parse(v, 3, argv) == -1, "next-value starting with -- is rejected");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_MISSING_VALUE &&
             strcmp(err->key, "tags") == 0,
         "missing value recorded for option-like next arg");
  vl_destroy(v);
}

void test_short_next_rejects_option_lookalike(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "tag", .short_name = 't', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"-t",
      (char *)"-x",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for short lookalike rejection");
  EXPECT(vl_parse(v, 3, argv) == -1, "short next-value -x rejected");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_MISSING_VALUE,
         "short missing value recorded for option-like next arg");
  vl_destroy(v);
}

void test_literal_enable_option_name(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "enable-tracing", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--enable-tracing=verbose",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for literal enable- option");
  EXPECT(vl_parse(v, 2, argv) == 0, "literal enable-* option name parses");
  value = vl_get(v, "enable-tracing");
  EXPECT(value && value->kind == VL_VALUE_STRING &&
             strcmp(value->raw, "verbose") == 0,
         "literal enable- option captures value");
  vl_destroy(v);
}

void test_enable_on_non_ed_option(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "tracing", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--enable-tracing",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for --enable- on non-ED option");
  EXPECT(vl_parse(v, 2, argv) == -1, "enable- on non-ED option fails");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNKNOWN_OPTION &&
             strcmp(err->key, "enable-tracing") == 0,
         "enable- on non-toggle records UNKNOWN_OPTION with full key");
  vl_destroy(v);
}

void test_next_negative_int(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "limit", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--limit",
      (char *)"-1",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for next negative int");
  EXPECT(vl_parse(v, 3, argv) == 0, "next form accepts negative int");
  value = vl_get(v, "limit");
  EXPECT(value && value->kind == VL_VALUE_INT && value->as.integer == -1,
         "--limit -1 parses to -1");
  vl_destroy(v);
}

void test_next_negative_double(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "ratio", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOUBLE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--ratio",
      (char *)"-1.5",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for next negative double");
  EXPECT(vl_parse(v, 3, argv) == 0, "next form accepts negative double");
  value = vl_get(v, "ratio");
  EXPECT(value && value->kind == VL_VALUE_DOUBLE && value->as.number < -1.49 &&
             value->as.number > -1.51,
         "--ratio -1.5 parses to -1.5");
  vl_destroy(v);
}

void test_short_next_negative_number(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "limit", .short_name = 'l', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_INT, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"-l",
      (char *)"-42",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for short next negative int");
  EXPECT(vl_parse(v, 3, argv) == 0, "short next accepts negative int");
  value = vl_get(v, "limit");
  EXPECT(value && value->kind == VL_VALUE_INT && value->as.integer == -42,
         "-l -42 parses to -42");
  vl_destroy(v);
}

void test_next_string_still_rejects_dash_value(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "name", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--name",
      (char *)"-foo",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for string still rejects dash");
  EXPECT(vl_parse(v, 3, argv) == -1,
         "string-typed next still rejects -foo lookalike");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_MISSING_VALUE,
         "string-typed dash value records missing value");
  vl_destroy(v);
}

void test_double_rejects_nan(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "ratio", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOUBLE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--ratio=nan",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for nan rejection");
  EXPECT(vl_parse(v, 2, argv) == -1, "double parser rejects nan");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE &&
             strcmp(err->key, "ratio") == 0,
         "nan records invalid value");
  vl_destroy(v);
}

void test_double_rejects_inf(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "ratio", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOUBLE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--ratio=inf",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for inf rejection");
  EXPECT(vl_parse(v, 2, argv) == -1, "double parser rejects inf");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE &&
             strcmp(err->key, "ratio") == 0,
         "inf records invalid value");
  vl_destroy(v);
}

void test_number_rejects_nan(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_NUMBER, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--n=nan",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for number rejects nan");
  EXPECT(vl_parse(v, 2, argv) == -1, "number parser rejects nan");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE,
         "nan in number records invalid value");
  vl_destroy(v);
}

void test_number_int_bounds_apply(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_NUMBER, .has_int_max = true,
             .int_max = 4, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--n=8",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for number int bounds");
  EXPECT(vl_parse(v, 2, argv) == -1, "number int above max fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE &&
             strcmp(err->message, "integer value is above maximum") == 0,
         "number int max records bounds error");
  vl_destroy(v);
}

void test_number_rejects_leading_space(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_NUMBER, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--n= 8",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for leading-space number");
  EXPECT(vl_parse(v, 2, argv) == -1, "number rejects leading space");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE,
         "leading-space number records invalid value");
  vl_destroy(v);
}

void test_malformed_negative_next_is_not_consumed(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "ratio", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOUBLE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--ratio",
      (char *)"-.x",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for malformed negative next value");
  EXPECT(vl_parse(v, 3, argv) == -1,
         "malformed negative next token is not consumed");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_MISSING_VALUE,
         "malformed negative next token records missing value");
  vl_destroy(v);
}

