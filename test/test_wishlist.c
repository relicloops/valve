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

/* bare --flag / -f boolean presence, =true / =false literals */
void test_bool_flags(void) {
  bool verbose = false;
  const vl_option_t *const options[] = {
      VL_OPT(.name = "verbose", .short_name = 'V',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL, .data = &verbose, .offset = 0,
             .target = VL_TARGET_BOOL, ),
      NULL,
  };
  char *argv_bare[] = {(char *)"valve", (char *)"--verbose"};
  valve_t *v = parser_(options, 1);

  EXPECT(v != NULL, "parser for bool flags");
  EXPECT(vl_parse(v, 2, argv_bare) == 0, "bare bool flag parses");
  EXPECT(verbose, "bare bool flag targets true");
  vl_destroy(v);

  verbose = false;
  char *argv_short[] = {(char *)"valve", (char *)"-V"};
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, argv_short) == 0, "bare short bool flag parses");
  EXPECT(verbose, "bare short bool flag targets true");
  vl_destroy(v);

  verbose = true;
  char *argv_false[] = {(char *)"valve", (char *)"--verbose=false"};
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, argv_false) == 0, "bool flag =false parses");
  EXPECT(!verbose, "bool flag =false targets false");
  vl_destroy(v);

  char *argv_bad[] = {(char *)"valve", (char *)"--verbose=maybe"};
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, argv_bad) != 0, "bool flag rejects non-literal");
  vl_destroy(v);
}

/* .required enforcement, and reserved tokens skipping it */
void test_required(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "port", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT, .required = true, ),
      VL_OPT(.name = "label", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  char *argv_missing[] = {(char *)"valve", (char *)"--label=x"};
  valve_t *v = parser_(options, 2);

  EXPECT(v != NULL, "parser for required options");
  EXPECT(vl_parse(v, 2, argv_missing) != 0, "missing required fails parse");
  EXPECT(vl_error_count(v) == 1 &&
             vl_error_at(v, 0)->code == VL_ERROR_MISSING_REQUIRED,
         "missing required records structured error");
  vl_destroy(v);

  char *argv_ok[] = {(char *)"valve", (char *)"--port=8080"};
  v = parser_(options, 2);
  EXPECT(vl_parse(v, 2, argv_ok) == 0, "provided required passes");
  vl_destroy(v);

  char *argv_help[] = {(char *)"valve", (char *)"--help"};
  v = parser_(options, 2);
  EXPECT(vl_parse(v, 2, argv_help) == 0 && vl_reserved_fired(v),
         "reserved token skips required check");
  vl_destroy(v);
}

/* sub-verb dispatch, option scoping, accessor */
void test_subverbs(void) {
  const vl_option_t *const add_options[] = {
      VL_OPT(.name = "name", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .required = true, ),
      NULL,
  };
  const vl_verb_t *const page_subverbs[] = {
      VL_CMD(.name = "add", .options = add_options, ),
      VL_CMD(.name = "remove", ),
      NULL,
  };
  const vl_option_t *const page_options[] = {
      VL_OPT(.name = "website", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, ),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "page", .options = page_options,
             .verbs = page_subverbs, ),
      NULL,
  };
  const vl_executable_t settings = {.verbs = verbs};
  char *argv_ok[] = {(char *)"valve", (char *)"page", (char *)"add",
                     (char *)"--name=about", (char *)"--website=x"};
  valve_t *v = vl_create(&settings);

  EXPECT(v != NULL, "parser for sub-verbs");
  EXPECT(vl_parse(v, 5, argv_ok) == 0, "sub-verb parse succeeds");
  EXPECT(vl_verb_get(v) && strcmp(vl_verb_get(v), "page") == 0,
         "verb get returns parent");
  EXPECT(vl_subverb_get(v) && strcmp(vl_subverb_get(v), "add") == 0,
         "subverb get returns leaf");
  EXPECT(vl_has(v, "name") && vl_has(v, "website"),
         "sub-verb and parent options both resolve");
  vl_destroy(v);

  char *argv_missing[] = {(char *)"valve", (char *)"page"};
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, argv_missing) != 0, "missing sub-verb fails");
  EXPECT(vl_error_count(v) == 1 &&
             vl_error_at(v, 0)->code == VL_ERROR_MISSING_COMMAND,
         "missing sub-verb records structured error");
  vl_destroy(v);

  char *argv_unknown[] = {(char *)"valve", (char *)"page", (char *)"drop"};
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, argv_unknown) != 0, "unknown sub-verb fails");
  EXPECT(vl_error_count(v) == 1 &&
             vl_error_at(v, 0)->code == VL_ERROR_UNKNOWN_COMMAND,
         "unknown sub-verb records structured error");
  vl_destroy(v);

  char *argv_required[] = {(char *)"valve", (char *)"page", (char *)"add"};
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, argv_required) != 0,
         "sub-verb required option enforced");
  vl_destroy(v);

  char *argv_scope[] = {(char *)"valve", (char *)"page", (char *)"remove",
                        (char *)"--name=about"};
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 4, argv_scope) != 0,
         "sibling sub-verb option is out of scope");
  vl_destroy(v);
}

