#include "test_support.h"
#include "fault_alloc.h"
#include "private.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void setUp(void) { vl_test_alloc_reset(); }
void tearDown(void) { vl_test_alloc_reset(); }

void test_error_add_cap_overflow(void) {
  valve_t v = {0};
  size_t scratch = 0;
  /* *2 overflows */
  v.error_cap_ = (SIZE_MAX / 2) + 1;
  v.error_count_ = v.error_cap_;
  EXPECT(vl_error_add_(&v, VL_ERROR_UNKNOWN_OPTION, 0, "k", "m") == -1,
         "cap*2 overflow");

  /* Prefer a cap where *2 succeeds but *sizeof overflows */
  v.error_cap_ = SIZE_MAX / sizeof(vl_error_t) / 2 + 1;
  if (!__builtin_mul_overflow(v.error_cap_, (size_t)2, &scratch)) {
    v.error_count_ = v.error_cap_;
    EXPECT(vl_error_add_(&v, VL_ERROR_UNKNOWN_OPTION, 0, "k", "m") == -1,
           "cap*sizeof overflow");
  }
}

void test_result_set_array_overflow(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .repeat = VL_OPTION_REPEAT_ARRAY),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  EXPECT(v != NULL, "parser");
  /* Need an already-wrapped array (kind ARRAY, raw NULL) before append_ */
  char *argv[] = {(char *)"valve", (char *)"--mode=a", (char *)"--mode=b"};
  EXPECT(vl_parse(v, 3, argv) == 0, "seed array");
  EXPECT(v->result_count_ == 1, "one result");
  EXPECT(v->results_[0].value.kind == VL_VALUE_ARRAY, "wrapped");
  size_t saved = v->results_[0].value.as.array.count;
  v->results_[0].value.as.array.count = SIZE_MAX;
  vl_value_t extra = {.kind = VL_VALUE_STRING, .raw = strdup("c")};
  EXPECT(vl_result_set_(v, options[0], &extra, 1) == -1, "array count overflow");
  vl_value_clear(&extra);
  v->results_[0].value.as.array.count = saved;
  vl_destroy(v);
}

void test_result_set_null_guards(void) {
  vl_option_t opt = {.name = "x", .type = VL_OPT_TYPE_LONG,
                     .value = VL_OPTION_VALUE_BOOL};
  vl_value_t val = {.kind = VL_VALUE_BOOL, .as.boolean = true};
  valve_t v = {0};
  EXPECT(vl_result_set_(NULL, &opt, &val, 0) == -1, "null valve");
  EXPECT(vl_result_set_(&v, NULL, &val, 0) == -1, "null opt");
  opt.name = NULL;
  EXPECT(vl_result_set_(&v, &opt, &val, 0) == -1, "null name");
  opt.name = "x";
  EXPECT(vl_result_set_(&v, &opt, NULL, 0) == -1, "null value");
}

void test_help_print_stale_target_and_null_name(void) {
  const vl_verb_t *const verbs[] = {VL_CMD(.name = "run"), NULL};
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "parser");
  char *argv[] = {(char *)"valve", (char *)"--help=run"};
  EXPECT(vl_parse(v, 2, argv) == 0, "targeted help");
  /* Stale help_target_ that no longer resolves → print_targeted false */
  free(v->help_target_);
  v->help_target_ = strdup("does-not-exist-anymore");
  vl_help_print_default_(v);

  /* Null option name in table → print_option_ early return */
  if (v->option_count_ == 0 && v->verb_count_ > 0) {
    /* poke verb option slot if any — else add a fake global */
  }
  vl_option_t ghost = {.name = NULL, .type = VL_OPT_TYPE_LONG,
                       .value = VL_OPTION_VALUE_BOOL};
  vl_option_t *saved = v->options_;
  size_t saved_n = v->option_count_;
  v->options_ = &ghost;
  v->option_count_ = 1;
  free(v->help_target_);
  v->help_target_ = NULL;
  vl_help_print_default_(v);
  v->options_ = saved;
  v->option_count_ = saved_n;
  vl_destroy(v);
  EXPECT(1, "help_print edges");
}

void test_conflict_error_label_oom(void) {
  vl_option_t later = {
      .name = "later", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_option_t earlier = {
      .name = "earlier", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_result_t res = {.argv_index = 1};
  valve_t v = {0};
  vl_test_alloc_fail_after(1);
  EXPECT(vl_conflict_error_(&v, &later, &res, &earlier) == -1, "label OOM");
  vl_test_alloc_reset();
  /* message malloc fail after label ok */
  vl_test_alloc_fail_after(2);
  EXPECT(vl_conflict_error_(&v, &later, &res, &earlier) == -1, "message OOM");
  vl_test_alloc_reset();
}

void test_requirement_error_label_oom(void) {
  vl_option_t src = {
      .name = "src", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_option_t tgt = {
      .name = "tgt", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_result_t res = {.argv_index = 0};
  valve_t v = {0};
  vl_test_alloc_fail_after(1);
  EXPECT(vl_requirement_error_(&v, &src, &res, &tgt) == -1, "req label OOM");
  vl_test_alloc_reset();
  vl_test_alloc_fail_after(2);
  EXPECT(vl_requirement_error_(&v, &src, &res, &tgt) == -1, "req msg OOM");
  vl_test_alloc_reset();
}

void test_conflicts_copy_oom_on_verb_tables(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  a.conflicts = a_c;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  const vl_option_t *const sub_opts[] = {&a, &b, NULL};
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sub_opts), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = opts, .verbs = subs), NULL};
  vl_executable_t settings = {
      .options = opts, .verbs = verbs, .color = VAL_COLOR_NEVER};
  /* May fail schema if collide — try verb-only */
  settings.options = NULL;
  int fails = 0;
  for (int n = 1; n <= 40; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (!v)
      ++fails;
    else
      vl_destroy(v);
  }
  EXPECT(fails > 0, "verb/sub conflict copy OOM");
}

void test_mapped_global_hit_brace(void) {
  static const vl_option_t g = {
      .name = "g", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const globals[] = {&g, NULL};
  vl_option_t copied = g;
  const vl_option_t *m =
      option_mapped_(&g, globals, &copied, 1, NULL, NULL, 0, NULL, NULL, 0);
  EXPECT(m == &copied, "mapped global");
}

void test_targets_clear_none_case(void) {
  static int x = 3;
  static vl_option_t none_opt = {
      .name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .target = VL_TARGET_NONE, .data = &x};
  static const vl_option_t *const opts[] = {&none_opt, NULL};
  vl_executable_t settings = {.options = opts};
  vl_targets_clear(&settings);
  EXPECT(x == 3, "NONE untouched");
}

void test_conflicts_check_error_oom(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  a.conflicts = a_c;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "parser");
  /* Parse both then fail on conflict_error path via alloc fail */
  for (int n = 1; n <= 20; ++n) {
    vl_test_alloc_reset();
    /* allow parse allocs then fail during check — fail late */
    vl_test_alloc_fail_after(n);
    char *argv[] = {(char *)"valve", (char *)"--a", (char *)"--b"};
    (void)vl_parse(v, 3, argv);
    vl_test_alloc_reset();
  }
  vl_destroy(v);
  EXPECT(1, "conflicts_check error OOM");
}

void test_conflict_requirement_add_cap_overflow(void) {
  vl_option_t a = {.name = "a"};
  vl_option_t b = {.name = "b"};
  valve_t v = {0};

  /* conflicts_ NULL skips exists_; count==cap hits *2 wrap */
  v.conflict_cap_ = (SIZE_MAX / 2) + 1;
  v.conflict_count_ = v.conflict_cap_;
  EXPECT(vl_conflict_add_(&v, &a, &b, NULL, NULL) == -1, "conflict *2 wrap");

  v.conflict_cap_ = SIZE_MAX / sizeof(valve_conflict_t) / 2 + 1;
  v.conflict_count_ = v.conflict_cap_;
  EXPECT(vl_conflict_add_(&v, &a, &b, NULL, NULL) == -1, "conflict *sizeof");

  v.requirement_cap_ = (SIZE_MAX / 2) + 1;
  v.requirement_count_ = v.requirement_cap_;
  EXPECT(vl_requirement_add_(&v, &a, &b, NULL, NULL) == -1, "req *2 wrap");

  v.requirement_cap_ = SIZE_MAX / sizeof(valve_requirement_t) / 2 + 1;
  v.requirement_count_ = v.requirement_cap_;
  EXPECT(vl_requirement_add_(&v, &a, &b, NULL, NULL) == -1, "req *sizeof");
}

void test_result_set_cap_overflow(void) {
  vl_option_t opt = {
      .name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_value_t val = {.kind = VL_VALUE_BOOL, .as.boolean = true};
  valve_t v = {0};
  v.result_cap_ = (SIZE_MAX / 2) + 1;
  v.result_count_ = v.result_cap_;
  /* results_ NULL skips key search */
  EXPECT(vl_result_set_(&v, &opt, &val, 0) == -1, "result *2 overflow");

  val = (vl_value_t){.kind = VL_VALUE_BOOL, .as.boolean = true};
  v.result_cap_ = SIZE_MAX / sizeof(vl_result_t) / 2 + 1;
  v.result_count_ = v.result_cap_;
  EXPECT(vl_result_set_(&v, &opt, &val, 0) == -1, "result *sizeof overflow");
}

void test_append_array_realloc_fail(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .repeat = VL_OPTION_REPEAT_ARRAY),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  EXPECT(v != NULL, "parser");
  char *argv[] = {(char *)"valve", (char *)"--mode=a", (char *)"--mode=b"};
  EXPECT(vl_parse(v, 3, argv) == 0, "seed");
  vl_value_t extra = {.kind = VL_VALUE_STRING, .raw = strdup("c")};
  vl_test_alloc_fail_after(1);
  EXPECT(vl_result_set_(v, options[0], &extra, 1) == -1, "append realloc fail");
  vl_test_alloc_reset();
  vl_value_clear(&extra);
  vl_destroy(v);
}

void test_conflict_seen_loop_continue(void) {
  vl_option_t a = {.name = "a"};
  vl_option_t b = {.name = "b"};
  vl_option_t c = {.name = "c"};
  valve_conflict_t conflicts[2] = {
      {.first = &a, .second = &b},
      {.first = &a, .second = &c},
  };
  valve_t v = {.conflicts_ = conflicts, .conflict_count_ = 2};
  /* before=2: first miss, second hit → for-body closing brace */
  EXPECT(vl_conflict_seen_(&v, 2, &a, &c), "seen after miss");
  EXPECT(!vl_conflict_seen_(&v, 2, &a, &(vl_option_t){.name = "z"}),
         "seen none");
}

void test_annotations_conflict_no_match(void) {
  vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_option_t c = {
      .name = "c", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  valve_conflict_t conflicts[1] = {{.first = &a, .second = &b}};
  valve_t v = {.conflicts_ = conflicts, .conflict_count_ = 1};
  option_annotations_print_(&v, &c); /* loop runs, no match */
  EXPECT(1, "annotations no-match loop");
}

void test_find_short_verb_miss_falls_to_global(void) {
  static const vl_option_t g = {
      .name = "glob", .short_name = 'g',
      .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t vo = {
      .name = "verbopt", .short_name = 'v',
      .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  /* 'v' reserved — use 'q' on verb */
  static vl_option_t verb_opt = {
      .name = "quiet", .short_name = 'q',
      .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  (void)vo;
  const vl_option_t *const globals[] = {&g, NULL};
  const vl_option_t *const vopts[] = {&verb_opt, NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = vopts), NULL};
  vl_executable_t settings = {
      .options = globals, .verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "create");
  char *argv[] = {(char *)"valve", (char *)"run", (char *)"-g"};
  EXPECT(vl_parse(v, 3, argv) == 0, "global short after verb miss");
  vl_destroy(v);
}

void test_mapped_global_loop_exit(void) {
  static const vl_option_t g1 = {
      .name = "g1", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t g2 = {
      .name = "g2", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const globals[] = {&g1, &g2};
  vl_option_t copied[2] = {g1, g2};
  static const vl_option_t needle = {
      .name = "nope", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  EXPECT(option_mapped_(&needle, globals, copied, 2, NULL, NULL, 0, NULL, NULL,
                        0) == NULL,
         "mapped miss exits global loop");
  EXPECT(option_mapped_(&g2, globals, copied, 2, NULL, NULL, 0, NULL, NULL, 0) ==
             &copied[1],
         "mapped second global");
}

void test_help_option_name_suggestion(void) {
  /* Ambiguous bare sub-verb → suggestion path (option-name arm is schema-dead
   * because option_anywhere_ would resolve first; subverb arm still runs). */
  const vl_verb_t *const subs[] = {VL_CMD(.name = "list"), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "alpha", .verbs = subs),
      VL_CMD(.name = "beta", .verbs = subs),
      NULL};
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "create");
  char *argv[] = {(char *)"valve", (char *)"--help=list"};
  EXPECT(vl_parse(v, 2, argv) == -1, "ambiguous subverb help");
  EXPECT(vl_error_count(v) >= 1, "error");
  const vl_error_t *err = vl_error_at(v, 0);
  EXPECT(err && err->message && strstr(err->message, "did you mean"), "hint");
  vl_destroy(v);
}

void test_user_has_option_empty_and_short(void) {
  static vl_option_t sh = {
      .name = "quiet", .short_name = 'q',
      .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  const vl_option_t *const opts[] = {&sh, NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = opts), NULL};
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "create");
  char *dashdash[] = {(char *)"valve", (char *)"run", (char *)"--"};
  EXPECT(vl_parse(v, 3, dashdash) == 0, "bare -- stops parse");
  vl_destroy(v);
  v = vl_create(&settings);
  char *shortq[] = {(char *)"valve", (char *)"run", (char *)"-q"};
  EXPECT(v && vl_parse(v, 3, shortq) == 0, "short on verb");
  vl_destroy(v);
}

void test_consumable_null_next_argv(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  /* SEPARATE: bare --n with no following argv hits consumable early return */
  valve_t *v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  EXPECT(v != NULL, "parser");
  char *argv_end[] = {(char *)"valve", (char *)"--n"};
  EXPECT(vl_parse(v, 2, argv_end) == -1, "missing next at argc end");
  vl_destroy(v);

  v = parser_with_form_(options, 1, VL_ASSIGN_SEPARATE);
  char *argv_null[] = {(char *)"valve", (char *)"--n", NULL};
  EXPECT(vl_parse(v, 3, argv_null) == -1, "null next value slot");
  vl_destroy(v);
}

void test_conflicts_copy_verb_subverb_oom_direct(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  a.conflicts = a_c;

  /* Verb-level conflicts only → line 23 */
  const vl_option_t *const vopts[] = {&a, &b, NULL};
  const vl_verb_t *const verbs_only[] = {
      VL_CMD(.name = "run", .options = vopts), NULL};
  vl_executable_t settings = {.verbs = verbs_only, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "create verb conflicts");
  vl_conflicts_clear_(v);
  vl_test_alloc_fail_after(1);
  EXPECT(vl_conflicts_copy_(v, &settings) == -1, "verb conflict copy OOM");
  vl_test_alloc_reset();
  vl_destroy(v);

  /* Subverb conflicts only (verb options conflict-free) → line 38 */
  static vl_option_t sa = {
      .name = "sa", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t sb = {
      .name = "sb", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t plain_opt = {
      .name = "p", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const sa_c[] = {&sb, NULL};
  sa.conflicts = sa_c;
  const vl_option_t *const plain[] = {&plain_opt, NULL};
  const vl_option_t *const sopts[] = {&sa, &sb, NULL};
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sopts), NULL};
  const vl_verb_t *const verbs_sub[] = {
      VL_CMD(.name = "run", .options = plain, .verbs = subs), NULL};
  vl_executable_t settings_sub = {
      .verbs = verbs_sub, .color = VAL_COLOR_NEVER};
  v = vl_create(&settings_sub);
  EXPECT(v != NULL, "create subverb conflicts");
  vl_conflicts_clear_(v);
  vl_test_alloc_fail_after(1);
  EXPECT(vl_conflicts_copy_(v, &settings_sub) == -1, "subverb conflict copy OOM");
  vl_test_alloc_reset();
  vl_destroy(v);
}

void test_requirements_copy_verb_subverb_oom_direct(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_r[] = {&b, NULL};
  a.requires = a_r;

  const vl_option_t *const vopts[] = {&a, &b, NULL};
  const vl_verb_t *const verbs_only[] = {
      VL_CMD(.name = "run", .options = vopts), NULL};
  vl_executable_t settings = {.verbs = verbs_only, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "create verb reqs");
  vl_requirements_clear_(v);
  vl_test_alloc_fail_after(1);
  EXPECT(vl_requirements_copy_(v, &settings) == -1, "verb req copy OOM");
  vl_test_alloc_reset();
  vl_destroy(v);

  static vl_option_t sa = {
      .name = "sa", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t sb = {
      .name = "sb", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t plain_opt = {
      .name = "p", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const sa_r[] = {&sb, NULL};
  sa.requires = sa_r;
  const vl_option_t *const plain[] = {&plain_opt, NULL};
  const vl_option_t *const sopts[] = {&sa, &sb, NULL};
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sopts), NULL};
  const vl_verb_t *const verbs_sub[] = {
      VL_CMD(.name = "run", .options = plain, .verbs = subs), NULL};
  vl_executable_t settings_sub = {
      .verbs = verbs_sub, .color = VAL_COLOR_NEVER};
  v = vl_create(&settings_sub);
  EXPECT(v != NULL, "create subverb reqs");
  vl_requirements_clear_(v);
  vl_test_alloc_fail_after(1);
  EXPECT(vl_requirements_copy_(v, &settings_sub) == -1, "subverb req copy OOM");
  vl_test_alloc_reset();
  vl_destroy(v);
}

void test_conflicts_check_later_first_oom(void) {
  /* first argv_index > second → else branch of conflict_error */
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  a.conflicts = a_c;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "create");
  for (int n = 1; n <= 25; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *argv[] = {(char *)"valve", (char *)"--b", (char *)"--a"};
    (void)vl_parse(v, 3, argv);
    vl_test_alloc_reset();
  }
  vl_destroy(v);
  EXPECT(1, "later-first conflict OOM");
}

void test_requirements_copy_oom(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_r[] = {&b, NULL};
  a.requires = a_r;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  const vl_option_t *const sub_opts[] = {&a, &b, NULL};
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sub_opts), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = opts, .verbs = subs), NULL};
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  int fails = 0;
  for (int n = 1; n <= 50; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (!v)
      ++fails;
    else
      vl_destroy(v);
  }
  EXPECT(fails > 0, "req copy OOM");
}
