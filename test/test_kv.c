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


void setUp(void) {}
void tearDown(void) {}

void test_kv_equals(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "kv-flag", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_KV, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--kv-flag=!key:value|key1:simboltodefineamulti",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for kv equals");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse --kv-flag=!k:v");
  value = vl_get(v, "kv-flag");
  EXPECT(value && value->kind == VL_VALUE_KV && value->as.kv.count == 2,
         "kv equals stores pair list");
  EXPECT(value && strcmp(value->as.kv.pairs[0].key, "key") == 0 &&
             value->as.kv.pairs[0].value.kind == VL_VALUE_STRING &&
             strcmp(value->as.kv.pairs[0].value.raw, "value") == 0,
         "kv first pair is correct");
  EXPECT(value && strcmp(value->as.kv.pairs[1].key, "key1") == 0 &&
             value->as.kv.pairs[1].value.kind == VL_VALUE_STRING &&
             strcmp(value->as.kv.pairs[1].value.raw, "simboltodefineamulti") ==
                 0,
         "kv second pair is correct");
  vl_destroy(v);
}

void test_kv_next(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "kv", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_KV, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--kv",
      (char *)"!a:b|c:d",
  };
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  const vl_value_t *value = NULL;

  EXPECT(v != NULL, "parser for kv next");
  EXPECT(vl_parse(v, 3, argv) == 0, "parse --kv !k:v");
  value = vl_get(v, "kv");
  EXPECT(value && value->kind == VL_VALUE_KV && value->as.kv.count == 2 &&
             strcmp(value->as.kv.pairs[1].key, "c") == 0 &&
             value->as.kv.pairs[1].value.kind == VL_VALUE_STRING &&
             strcmp(value->as.kv.pairs[1].value.raw, "d") == 0,
         "kv next stores pair list");
  vl_destroy(v);
}

void test_kv_tree_and_auto_scalars(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "kv", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_KV, ),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--kv=!proxy:{!lane:\"api.xxx.loop\"|cpus:2|enabled:true|label:"
              "\"true\"|workers:\"2\"}|ratio:1.5",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *value = NULL;
  const vl_value_t *proxy = NULL;

  EXPECT(v != NULL, "parser for kv tree");
  EXPECT(vl_parse(v, 2, argv) == 0, "parse nested kv tree");
  value = vl_get(v, "kv");
  EXPECT(value && value->kind == VL_VALUE_KV && value->as.kv.count == 2,
         "kv tree stores top-level pairs");
  proxy = value ? &value->as.kv.pairs[0].value : NULL;
  EXPECT(proxy && proxy->kind == VL_VALUE_KV && proxy->as.kv.count == 5,
         "kv tree stores nested pairs");
  EXPECT(proxy && proxy->as.kv.pairs[0].value.kind == VL_VALUE_STRING &&
             strcmp(proxy->as.kv.pairs[0].value.raw, "api.xxx.loop") == 0,
         "quoted kv lane stays string");
  EXPECT(proxy && proxy->as.kv.pairs[1].value.kind == VL_VALUE_INT &&
             proxy->as.kv.pairs[1].value.as.integer == 2,
         "unquoted kv integer coerces to int");
  EXPECT(proxy && proxy->as.kv.pairs[2].value.kind == VL_VALUE_BOOL &&
             proxy->as.kv.pairs[2].value.as.boolean == true,
         "unquoted kv true coerces to bool");
  EXPECT(proxy && proxy->as.kv.pairs[3].value.kind == VL_VALUE_STRING &&
             strcmp(proxy->as.kv.pairs[3].value.raw, "true") == 0,
         "quoted kv true stays string");
  EXPECT(proxy && proxy->as.kv.pairs[4].value.kind == VL_VALUE_STRING &&
             strcmp(proxy->as.kv.pairs[4].value.raw, "2") == 0,
         "quoted kv number stays string");
  EXPECT(value && value->as.kv.pairs[1].value.kind == VL_VALUE_DOUBLE &&
             value->as.kv.pairs[1].value.as.number > 1.49 &&
             value->as.kv.pairs[1].value.as.number < 1.51,
         "unquoted kv decimal coerces to double");
  vl_destroy(v);
}

