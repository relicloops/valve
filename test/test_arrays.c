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

void test_array_equals_autotyped(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mix", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--mix=hello,1,2.5,true",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for array equals");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse --flag=a,b,c,d");
  value = vl_get(v, "mix");
  EXPECT(value && value->kind == VL_VALUE_ARRAY && value->as.array.count == 4,
         "array equals stores four elements");
  EXPECT(value && value->as.array.items[0].kind == VL_VALUE_STRING &&
             strcmp(value->as.array.items[0].raw, "hello") == 0,
         "array element 0 is string");
  EXPECT(value && value->as.array.items[1].kind == VL_VALUE_INT &&
             value->as.array.items[1].as.integer == 1,
         "array element 1 is int");
  EXPECT(value && value->as.array.items[2].kind == VL_VALUE_DOUBLE &&
             value->as.array.items[2].as.number > 2.49 &&
             value->as.array.items[2].as.number < 2.51,
         "array element 2 is double");
  EXPECT(value && value->as.array.items[3].kind == VL_VALUE_BOOL &&
             value->as.array.items[3].as.boolean == true,
         "array element 3 is bool");
  vl_destroy(v);
}

void test_array_next_short(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "tags", .short_name = 't', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"-t",
      (char *)"alpha,beta,gamma",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for array short next");
  EXPECT(vl_parse(v, 3, argv) == 0, "parse -f a,b,c");
  value = vl_get(v, "tags");
  EXPECT(value && value->kind == VL_VALUE_ARRAY && value->as.array.count == 3,
         "short next array has three elements");
  EXPECT(value && strcmp(value->as.array.items[0].raw, "alpha") == 0 &&
             strcmp(value->as.array.items[2].raw, "gamma") == 0,
         "short next preserves order");
  vl_destroy(v);
}

void test_array_quoted_element(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "items", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--items=\"a,b\",1",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for quoted array element");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse quoted comma in array");
  value = vl_get(v, "items");
  EXPECT(value && value->kind == VL_VALUE_ARRAY && value->as.array.count == 2,
         "quoted element doesn't split on internal comma");
  EXPECT(value && value->as.array.items[0].kind == VL_VALUE_STRING &&
             strcmp(value->as.array.items[0].raw, "a,b") == 0,
         "quoted element kept as string with comma");
  EXPECT(value && value->as.array.items[1].kind == VL_VALUE_INT &&
             value->as.array.items[1].as.integer == 1,
         "unquoted int element after quoted element");
  vl_destroy(v);
}

void test_array_repeat_nests(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "groups", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY,
             .repeat = VL_OPTION_REPEAT_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--groups=a,b",
      (char *)"--groups=c,d",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for repeated array");
  EXPECT(vl_parse(v, 3, argv) == 0, "parse repeated array option");
  value = vl_get(v, "groups");
  EXPECT(value && value->kind == VL_VALUE_ARRAY && value->as.array.count == 2,
         "repeat wraps to outer array of two");
  EXPECT(value && value->as.array.items[0].kind == VL_VALUE_ARRAY &&
             value->as.array.items[0].as.array.count == 2 &&
             strcmp(value->as.array.items[0].as.array.items[0].raw, "a") == 0,
         "first inner array is [a,b]");
  EXPECT(value && value->as.array.items[1].kind == VL_VALUE_ARRAY &&
             value->as.array.items[1].as.array.count == 2 &&
             strcmp(value->as.array.items[1].as.array.items[1].raw, "d") == 0,
         "second inner array is [c,d]");
  vl_destroy(v);
}

void test_array_empty_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--x=",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for empty array");
  EXPECT(vl_parse(v, 2, argv) == -1, "empty array value fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE &&
             strcmp(err->key, "x") == 0,
         "empty array records structured error");
  vl_destroy(v);
}

void test_array_trailing_comma_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--x=a,b,",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for trailing comma");
  EXPECT(vl_parse(v, 2, argv) == -1, "trailing comma fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE,
         "trailing comma records invalid value");
  vl_destroy(v);
}

void test_array_repeat_target_value(void) {
  vl_value_t groups = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "g", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, .repeat = VL_OPTION_REPEAT_ARRAY,
             .data = &groups, .target = VL_TARGET_VALUE, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--g=a,b",
      (char *)"--g=c,d",
  };
  valve_t *v = parser_(options, 1);

  EXPECT(v != NULL, "parser for array repeat target");
  EXPECT(vl_parse(v, 3, argv) == 0, "parse repeated array option into target");
  vl_destroy(v);

  EXPECT(groups.kind == VL_VALUE_ARRAY && groups.as.array.count == 2,
         "target groups survives vl_destroy");
  EXPECT(groups.as.array.items[0].kind == VL_VALUE_ARRAY &&
             groups.as.array.items[0].as.array.count == 2 &&
             strcmp(groups.as.array.items[0].as.array.items[0].raw, "a") == 0,
         "target inner [0] is [a,b]");
  EXPECT(groups.as.array.items[1].kind == VL_VALUE_ARRAY &&
             groups.as.array.items[1].as.array.count == 2 &&
             strcmp(groups.as.array.items[1].as.array.items[1].raw, "d") == 0,
         "target inner [1] is [c,d]");

  vl_value_clear(&groups);
}

void test_array_double_comma_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--x=a,,b",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for double comma");
  EXPECT(vl_parse(v, 2, argv) == -1, "double comma fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE,
         "double comma records invalid value");
  vl_destroy(v);
}

void test_array_unterminated_quote_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--x=\"unterminated",
  };
  valve_t *v = parser_(options, 1);
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "parser for unterminated quote");
  EXPECT(vl_parse(v, 2, argv) == -1, "unterminated quote fails parse");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_INVALID_VALUE,
         "unterminated quote records invalid value");
  vl_destroy(v);
}

