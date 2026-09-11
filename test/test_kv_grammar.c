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

#include "fault_alloc.h"
#include "private.h"

#include <locale.h>

void setUp(void) { vl_test_alloc_reset(); }
void tearDown(void) { vl_test_alloc_reset(); }

static const vl_option_t *const kv_opts_[] = {
    VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
    VL_OPT(.name = "auto", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_AUTO),
    VL_OPT(.name = "arr", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_ARRAY),
    VL_OPT(.name = "d", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_DOUBLE),
    NULL,
};

/* Parse one inline literal for `name`; returns the parser (caller destroys)
 * and the parse result through *rc. */
static valve_t *parse_one_(const char *name, const char *literal, int *rc) {
  char arg[512];
  snprintf(arg, sizeof arg, "--%s=%s", name, literal);
  char *argv[] = {(char *)"valve", arg};
  valve_t *v = parser_(kv_opts_, 4);
  *rc = v ? vl_parse(v, 2, argv) : -1;
  return v;
}

static bool error_says_(const valve_t *v, const char *fragment) {
  const vl_error_t *err = vl_error_at(v, 0);
  return err && err->message && strstr(err->message, fragment) != NULL;
}

static const vl_kv_pair_t *pair_(const vl_value_t *map, size_t i) {
  return (map && map->kind == VL_VALUE_KV && i < map->as.kv.count)
             ? &map->as.kv.pairs[i]
             : NULL;
}

/* ---- accepted forms ---------------------------------------------------- */

void test_kv_whitespace_around_separators(void) {
  int rc;
  valve_t *v = parse_one_("meta", " a : 1 | b : { c : 25 | d : x } ", &rc);
  EXPECT(rc == 0, "whitespace around separators parses");
  const vl_value_t *m = vl_get(v, "meta");
  EXPECT(m && m->as.kv.count == 2, "two top-level pairs");
  EXPECT(pair_(m, 0) && strcmp(pair_(m, 0)->key, "a") == 0 &&
             pair_(m, 0)->value.kind == VL_VALUE_INT,
         "key without surrounding space, int value");
  const vl_value_t *b = pair_(m, 1) ? &pair_(m, 1)->value : NULL;
  EXPECT(b && b->kind == VL_VALUE_KV && b->as.kv.count == 2, "nested map");
  EXPECT(pair_(b, 1) && strcmp(pair_(b, 1)->value.raw, "x") == 0,
         "nested bare value trimmed");
  vl_destroy(v);
}

void test_kv_escapes_and_quoted_keys(void) {
  int rc;
  valve_t *v = parse_one_("meta", "\"k:1|2\":\"x\\\"y\\\\z\"|url:http://h/p", &rc);
  EXPECT(rc == 0, "escapes and quoted key parse");
  const vl_value_t *m = vl_get(v, "meta");
  EXPECT(pair_(m, 0) && strcmp(pair_(m, 0)->key, "k:1|2") == 0,
         "quoted key keeps separators");
  EXPECT(pair_(m, 0) && strcmp(pair_(m, 0)->value.raw, "x\"y\\z") == 0,
         "escapes resolved");
  EXPECT(pair_(m, 1) && pair_(m, 1)->value.kind == VL_VALUE_STRING &&
             strcmp(pair_(m, 1)->value.raw, "http://h/p") == 0,
         "bare value may contain ':'");
  vl_destroy(v);

  v = parse_one_("meta", "e:\"\"", &rc);
  EXPECT(rc == 0 && pair_(vl_get(v, "meta"), 0) &&
             strcmp(pair_(vl_get(v, "meta"), 0)->value.raw, "") == 0,
         "empty quoted string is a string");
  vl_destroy(v);
}

typedef struct literal_case {
  const char *literal;
  vl_value_kind_t kind;
} literal_case_t;

void test_scalar_classification_table(void) {
  static const literal_case_t cases[] = {
      {"true", VL_VALUE_BOOL},   {"false", VL_VALUE_BOOL},
      {"True", VL_VALUE_STRING}, {"0", VL_VALUE_INT},
      {"+5", VL_VALUE_INT},      {"-3", VL_VALUE_INT},
      {"1.5", VL_VALUE_DOUBLE},  {".5", VL_VALUE_DOUBLE},
      {"1.", VL_VALUE_DOUBLE},   {"-1e3", VL_VALUE_DOUBLE},
      {"1E+2", VL_VALUE_DOUBLE}, {"1e-2", VL_VALUE_DOUBLE},
      {"0x1e3f", VL_VALUE_STRING},
      {"0x10", VL_VALUE_STRING}, {"1e", VL_VALUE_STRING},
      {"e5", VL_VALUE_STRING},   {"1.2.3", VL_VALUE_STRING},
      {"+", VL_VALUE_STRING},    {".", VL_VALUE_STRING},
      {"inf", VL_VALUE_STRING},  {"nan", VL_VALUE_STRING},
      {"1_000", VL_VALUE_STRING}, {"12abc", VL_VALUE_STRING},
  };
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
    int rc;
    valve_t *v = parse_one_("auto", cases[i].literal, &rc);
    const vl_value_t *val = vl_get(v, "auto");
    EXPECT(rc == 0 && val && val->kind == cases[i].kind, cases[i].literal);
    vl_destroy(v);
  }
  EXPECT(strcmp(option_scalar_message_(OPTION_SCALAR_OK), "value is valid") == 0,
         "scalar ok message");
  EXPECT(strcmp(option_scalar_message_((option_scalar_status_t)99),
                "value is valid") == 0,
         "scalar default message");
}

void test_kv_strict_numbers_inside_map(void) {
  int rc;
  valve_t *v = parse_one_("meta", "h:0x1e3f|e:1e5|f:.5", &rc);
  EXPECT(rc == 0, "parses");
  const vl_value_t *m = vl_get(v, "meta");
  EXPECT(pair_(m, 0) && pair_(m, 0)->value.kind == VL_VALUE_STRING,
         "hex stays a string");
  EXPECT(pair_(m, 1) && pair_(m, 1)->value.kind == VL_VALUE_DOUBLE &&
             pair_(m, 1)->value.as.number == 100000.0,
         "exponent is a double");
  EXPECT(pair_(m, 2) && pair_(m, 2)->value.kind == VL_VALUE_DOUBLE,
         "leading dot is a double");
  vl_destroy(v);
}

/* ---- rejected forms, each with its offset ------------------------------ */

static void expect_error_(const char *literal, const char *fragment,
                          const char *offset_text) {
  int rc;
  valve_t *v = parse_one_("meta", literal, &rc);
  char name[256];
  snprintf(name, sizeof name, "reject <%s>", literal);
  EXPECT(rc == -1, name);
  snprintf(name, sizeof name, "<%s> says '%s'", literal, fragment);
  EXPECT(error_says_(v, fragment), name);
  snprintf(name, sizeof name, "<%s> at %s", literal, offset_text);
  EXPECT(error_says_(v, offset_text), name);
  vl_destroy(v);
}

void test_kv_syntax_errors_with_offsets(void) {
  expect_error_("", "expected key", "offset 0");
  expect_error_("a:1|", "expected key", "offset 4");
  expect_error_("a:1|{}", "expected key", "offset 4");
  expect_error_("a:{}", "expected key", "offset 3");
  expect_error_("|a:1", "expected key", "offset 0");
  expect_error_("a", "expected ':' after key", "offset 1");
  expect_error_("a\"b:1", "expected ':' after key", "offset 1");
  expect_error_("a 1", "expected ':' after key", "offset 2");
  expect_error_("a:", "expected value", "offset 2");
  expect_error_("a:|b:1", "expected value", "offset 2");
  expect_error_("a:\"hi", "unterminated quoted string", "offset 2");
  expect_error_("a:\"h\\ni\"", "unknown escape", "offset 5");
  expect_error_("a:\"h\\", "unknown escape", "offset 5");
  expect_error_("a:1 b:2", "expected '|' or end of value", "offset 4");
  expect_error_("a:x\"y\"", "expected '|' or end of value", "offset 3");
  expect_error_("a:x{b:1}", "expected '|' or end of value", "offset 3");
  expect_error_("a:{b:1}x", "expected '|' or end of value", "offset 7");
  expect_error_("a:{b:1", "expected '|' or '}'", "offset 6");
  expect_error_("a:{b:1 c:2}", "expected '|' or '}'", "offset 7");
  expect_error_("a:1|a:2", "duplicate key", "offset 4");
  expect_error_("a:{b:1|b:2}", "duplicate key", "offset 7");
  expect_error_("x:1|\"x\":2", "duplicate key", "offset 4");
  expect_error_("!a:1", "prefix was removed", "offset 0");
  expect_error_(" !a:1", "prefix was removed", "offset 1");
  expect_error_("a:{!b:1}", "prefix was removed", "offset 3");
  expect_error_("a:9223372036854775808", "does not fit 64 bits", "offset 2");
  expect_error_("a:{b:1e400}", "decimal value is out of range", "offset 5");
}

void test_kv_depth_limit_reports_offset(void) {
  char deep[512];
  size_t pos = 0;
  for (int i = 0; i < 40; ++i) {
    deep[pos++] = 'a';
    deep[pos++] = ':';
    deep[pos++] = '{';
  }
  deep[pos++] = 'z';
  deep[pos++] = ':';
  deep[pos++] = '1';
  for (int i = 0; i < 40; ++i)
    deep[pos++] = '}';
  deep[pos] = '\0';
  expect_error_(deep, "nesting deeper than 32 levels", "offset 96");
}

void test_kv_messages_cover_every_status(void) {
  EXPECT(strstr(option_kv_message_(OPTION_KV_OK), "key:value") != NULL, "ok");
  EXPECT(strstr(option_kv_message_(OPTION_KV_OUT_OF_MEMORY), "memory") != NULL,
         "oom");
  EXPECT(strstr(option_kv_message_((option_kv_status_t)99), "key:value") != NULL,
         "default");
}

/* ---- range errors on the sibling paths --------------------------------- */

void test_auto_and_array_range_errors(void) {
  int rc;
  valve_t *v = parse_one_("auto", "99999999999999999999", &rc);
  EXPECT(rc == -1 && error_says_(v, "does not fit 64 bits"), "AUTO int range");
  vl_destroy(v);

  v = parse_one_("auto", "1e400", &rc);
  EXPECT(rc == -1 && error_says_(v, "decimal value is out of range"),
         "AUTO double range");
  vl_destroy(v);

  v = parse_one_("arr", "1,99999999999999999999", &rc);
  EXPECT(rc == -1 && error_says_(v, "does not fit 64 bits"), "array int range");
  vl_destroy(v);

  v = parse_one_("arr", "a,\"1e400\",1e400", &rc);
  EXPECT(rc == -1 && error_says_(v, "decimal value is out of range"),
         "array double range after a quoted one");
  vl_destroy(v);

  v = parse_one_("arr", "\"x\",0x1e", &rc);
  const vl_value_t *arr = vl_get(v, "arr");
  EXPECT(rc == 0 && arr && arr->as.array.count == 2 &&
             arr->as.array.items[1].kind == VL_VALUE_STRING,
         "array hex element stays a string");
  vl_destroy(v);
}

/* ---- locale independence ----------------------------------------------- */

void test_numbers_ignore_host_locale(void) {
  const char *previous = setlocale(LC_ALL, "de_DE.UTF-8");
  int rc;

  valve_t *v = parse_one_("meta", "r:1.5", &rc);
  EXPECT(rc == 0 && pair_(vl_get(v, "meta"), 0) &&
             pair_(vl_get(v, "meta"), 0)->value.kind == VL_VALUE_DOUBLE &&
             pair_(vl_get(v, "meta"), 0)->value.as.number == 1.5,
         "KV decimal under a comma locale");
  vl_destroy(v);

  v = parse_one_("d", "2.25", &rc);
  EXPECT(rc == 0 && vl_get(v, "d") && vl_get(v, "d")->as.number == 2.25,
         "typed double under a comma locale");
  vl_destroy(v);

  EXPECT(option_c_locale_() != (locale_t)0, "C locale available");
  (void)previous;
  setlocale(LC_ALL, "C");
}

/* ---- allocation failures ----------------------------------------------- */

void test_kv_allocation_failures_are_clean(void) {
  const char *literal = "\"q\\\"\":\"v\\\\\"|b:{c:1|d:2.5|e:true}|f:x";
  int failed = 0;
  for (int n = 1; n <= 40; ++n) {
    int rc;
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = parse_one_("meta", literal, &rc);
    vl_test_alloc_reset();
    if (rc != 0)
      ++failed;
    vl_destroy(v);
  }
  EXPECT(failed > 0, "some allocation points fail");
}
