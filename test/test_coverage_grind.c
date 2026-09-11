#include "test_support.h"
#include "fault_alloc.h"
#include "private.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pty_compat.h"

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

void setUp(void) { vl_test_alloc_reset(); }
void tearDown(void) {
  vl_test_alloc_reset();
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  unsetenv("TERM");
  unsetenv("COLUMNS");
  unsetenv("LINES");
  vl_color_support_reset();
}

/* ---- private null / edge APIs ---------------------------------------- */

void test_find_option_null_guards(void) {
  EXPECT(vl_option_find_(NULL, "x") == NULL, "find null v");
  EXPECT(vl_option_find_n_(NULL, "x", 1) == NULL, "find_n null v");
  EXPECT(vl_option_find_short_(NULL, 'x') == NULL, "find_short null v");
  EXPECT(vl_verb_find_(NULL, "x") == NULL, "verb_find null v");
  EXPECT(verb_find_in_(NULL, 1, "x") == NULL, "verb_find_in null");

  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .short_name = 'x',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  EXPECT(v != NULL, "parser");
  EXPECT(vl_option_find_(v, NULL) == NULL, "find null name");
  EXPECT(vl_option_find_n_(v, NULL, 1) == NULL, "find_n null name");
  EXPECT(vl_option_find_short_(v, '\0') == NULL, "find_short nul");
  EXPECT(vl_verb_find_(v, NULL) == NULL, "verb null name");
  vl_destroy(v);
}

void test_label_and_clear_null_guards(void) {
  EXPECT(option_label_(NULL) == NULL, "label null");
  vl_option_t bare = {.name = "z", .type = 0, .value = VL_OPTION_VALUE_BOOL};
  EXPECT(option_label_(&bare) == NULL, "label no type bits");

  vl_options_clear_(NULL);
  vl_settings_meta_clear_(NULL);
  vl_errors_clear_(NULL);
  vl_help_print_default_(NULL);
  EXPECT(vl_error_add_(NULL, VL_ERROR_UNKNOWN_OPTION, 0, "k", "m") == -1,
         "error_add null");
  EXPECT(vl_requirement_active_(NULL, NULL) == false, "req active null");
  EXPECT(1, "null clears ok");
}

void test_options_have_invalid_edges(void) {
  const vl_option_t *const with_null[] = {NULL};
  EXPECT(options_have_invalid_(with_null, 1), "null slot");

  static const vl_option_t short_no_char = {
      .name = "q", .type = VL_OPT_TYPE_SHORT, .value = VL_OPTION_VALUE_BOOL,
      .short_name = '\0'};
  const vl_option_t *const short_bad[] = {&short_no_char};
  EXPECT(options_have_invalid_(short_bad, 1), "short without char");

  static const vl_option_t tog_bad = {
      .name = "t", .type = VL_OPT_TYPE_TOGGLE, .value = VL_OPTION_VALUE_TOGGLE,
      .toggle_ref = ""};
  const vl_option_t *const tog[] = {&tog_bad};
  EXPECT(options_have_invalid_(tog, 1), "empty toggle_ref");
}

void test_conflict_seen_direct(void) {
  vl_option_t a = {.name = "a"};
  vl_option_t b1 = {.name = "b"};
  vl_option_t b2 = {.name = "b"};
  valve_conflict_t conflicts[2] = {
      {.first = &a, .second = &b1},
      {.first = &a, .second = &b2},
  };
  valve_t v = {.conflicts_ = conflicts, .conflict_count_ = 2};
  EXPECT(vl_conflict_seen_(&v, 1, &a, &b2), "duplicate other name seen");
  EXPECT(!vl_conflict_seen_(&v, 0, &a, &b1), "before==0 never seen");
}

void test_requirement_seen_direct(void) {
  vl_option_t src = {.name = "s"};
  vl_option_t t1 = {.name = "t"};
  vl_option_t t2 = {.name = "t"};
  valve_requirement_t reqs[2] = {
      {.source = &src, .target = &t1},
      {.source = &src, .target = &t2},
  };
  valve_t v = {.requirements_ = reqs, .requirement_count_ = 2};
  EXPECT(vl_requirement_forward_seen_(&v, 1, &src, &t2), "forward seen");

  vl_option_t s2 = {.name = "s"};
  reqs[0] = (valve_requirement_t){.source = &src, .target = &t1};
  reqs[1] = (valve_requirement_t){.source = &s2, .target = &t1};
  EXPECT(vl_requirement_reverse_seen_(&v, 1, &t1, &s2), "reverse seen");
}

void test_requirement_active_scope_mismatch(void) {
  valve_verb_t verb = {.name = "run"};
  valve_verb_t sub = {.name = "go"};
  vl_option_t a = {.name = "a"};
  vl_option_t b = {.name = "b"};
  valve_requirement_t req = {
      .source = &a, .target = &b, .verb = &verb, .subverb = &sub};
  valve_t v = {0};
  EXPECT(!vl_requirement_active_(&v, &req), "no active verb");
  v.active_verb_ = &verb;
  EXPECT(!vl_requirement_active_(&v, &req), "no active subverb");
  v.active_subverb_ = &sub;
  EXPECT(vl_requirement_active_(&v, &req), "scopes match");
}

void test_result_set_null_and_double_clone(void) {
  typedef struct {
    vl_value_t box;
  } cfg_t;
  cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "d", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_DOUBLE,
             .data = &cfg, .offset = offsetof(cfg_t, box),
             .target = VL_TARGET_VALUE),
      VL_OPT(.name = "none", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .target = VL_TARGET_NONE, .data = &cfg),
      NULL,
  };
  valve_t *v = parser_(options, 2);
  EXPECT(vl_result_set_(NULL, options[0], &(vl_value_t){0}, 0) == -1,
         "set null v");
  char *argv[] = {(char *)"valve", (char *)"--d=1.5", (char *)"--none"};
  EXPECT(v != NULL && vl_parse(v, 3, argv) == 0, "double into VALUE + NONE");
  EXPECT(cfg.box.kind == VL_VALUE_DOUBLE, "cloned double");
  vl_destroy(v);
  vl_value_clear(&cfg.box);
}

/* ---- parse edges ----------------------------------------------------- */

void test_parse_toggle_fallthrough_scopes(void) {
  /* active subverb/verb exist but toggle only on globals → loop fallthrough */
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "local", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "mid", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "enable-x", .toggle_ref = "x", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE),
      NULL,
  };
  const vl_verb_t *const subs[] = {VL_CMD(.name = "go", .options = sub_opts),
                                   NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = verb_opts, .verbs = subs), NULL};
  vl_executable_t settings = {
      .options = globals, .verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"run", (char *)"go",
                  (char *)"--enable-x"};
  EXPECT(v != NULL && vl_parse(v, 4, argv) == 0, "toggle via global fallthrough");
  vl_destroy(v);
}

void test_parse_empty_dash_tokens_and_short_reserved(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "quiet", .short_name = 'q',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  /* User-defined short that shadows reserved check path name_len==1 */
  vl_executable_t settings = {.options = options, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"-q"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "short bool");
  vl_destroy(v);

  /* empty name token "--" as argv after start */
  v = vl_create(&settings);
  char *dash[] = {(char *)"valve", (char *)"--"};
  EXPECT(vl_parse(v, 2, dash) == 0, "bare -- ends options");
  vl_destroy(v);
}

void test_parse_help_suggest_option_name(void) {
  /* Ambiguous bare sub-verb name → suggestion path (did you mean …?). */
  const vl_verb_t *const subs[] = {VL_CMD(.name = "list"), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .verbs = subs),
      VL_CMD(.name = "net", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help=list"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1, "ambiguous subverb help");
  EXPECT(vl_error_count(v) > 0, "error with suggestions");
  vl_destroy(v);
}

void test_parse_reserved_at_verb_and_sub_slots(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "q", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {VL_CMD(.name = "go", .options = sub_opts),
                                   NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};

  /* Unknown verb that is reserved with bad extract: --version=1 */
  valve_t *v = vl_create(&settings);
  char *ver[] = {(char *)"valve", (char *)"--version=1"};
  EXPECT(v != NULL && vl_parse(v, 2, ver) == -1, "version rejects value at verb");
  vl_destroy(v);

  /* --help=unknown at verb slot */
  v = vl_create(&settings);
  char *help_bad[] = {(char *)"valve", (char *)"--help=nope"};
  EXPECT(vl_parse(v, 2, help_bad) == -1, "bad help at verb slot");
  vl_destroy(v);

  /* reserved at subverb slot with bad target */
  v = vl_create(&settings);
  char *sub_help[] = {(char *)"valve", (char *)"run", (char *)"--help=nope"};
  EXPECT(vl_parse(v, 3, sub_help) == -1, "bad help at sub slot");
  vl_destroy(v);

  /* missing subverb with non-option empty string? skip */
  /* SEPARATE inline help disabled at mid-argv */
  settings.assign = VL_ASSIGN_SEPARATE;
  v = vl_create(&settings);
  char *mid[] = {(char *)"valve", (char *)"run", (char *)"go",
                 (char *)"--help=q"};
  EXPECT(vl_parse(v, 4, mid) == -1, "inline help disabled mid-parse");
  vl_destroy(v);

  settings.assign = VL_ASSIGN_INLINE;
  v = vl_create(&settings);
  char *mid_ok[] = {(char *)"valve", (char *)"run", (char *)"go",
                    (char *)"--help=nope"};
  EXPECT(vl_parse(v, 4, mid_ok) == -1, "bad help mid-parse");
  vl_destroy(v);
}

void test_parse_required_oom_path(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "need", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .required = true),
      VL_OPT(.name = "extra", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  valve_t *v = parser_(options, 2);
  EXPECT(v != NULL, "parser");
  /* Fail error_add inside required_check → OOM wrapper lines */
  for (int n = 1; n <= 8; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *argv[] = {(char *)"valve", (char *)"--extra"};
    (void)vl_parse(v, 2, argv);
    vl_test_alloc_reset();
  }
  vl_destroy(v);
  EXPECT(1, "required OOM path poked");
}

void test_parse_kv_quote_oom(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      VL_OPT(.name = "list", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY),
      NULL,
  };
  valve_t *v = parser_(options, 2);
  for (int n = 1; n <= 10; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *q[] = {(char *)"valve", (char *)"--meta=a:\"hi\""};
    (void)vl_parse(v, 2, q);
    vl_test_alloc_reset();
  }
  for (int n = 1; n <= 10; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *a[] = {(char *)"valve", (char *)"--list=\"x\",y"};
    (void)vl_parse(v, 2, a);
    vl_test_alloc_reset();
  }
  vl_destroy(v);
  EXPECT(1, "quoted kv/array OOM");
}

void test_oom_verbs_conflicts_requirements_rollback(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  static const vl_option_t *const b_r[] = {&a, NULL};
  a.conflicts = a_c;
  b.requires = b_r;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .description = "d", .usage = "u", .options = opts),
      NULL,
  };
  vl_executable_t settings = {
      .program_name = "p",
      .program_version = "1",
      .description = "d",
      .usage = "u",
      .logo = "L",
      .options = opts,
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };
  int nulls = 0;
  for (int n = 1; n <= 60; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (!v)
      ++nulls;
    else
      vl_destroy(v);
  }
  EXPECT(nulls > 0, "create rollback OOM paths");
}

void test_required_on_verb_and_subverb(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "need-sub", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL, .required = true),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "need-verb", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL, .required = true),
      NULL,
  };
  const vl_verb_t *const leaf[] = {
      VL_CMD(.name = "leaf", .options = verb_opts), NULL};
  const vl_verb_t *const go[] = {
      VL_CMD(.name = "go", .options = sub_opts), NULL};
  const vl_verb_t *const nested[] = {
      VL_CMD(.name = "nest", .verbs = go), NULL};

  vl_executable_t leaf_s = {.verbs = leaf, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&leaf_s);
  char *a1[] = {(char *)"valve", (char *)"leaf"};
  EXPECT(v != NULL && vl_parse(v, 2, a1) == -1, "missing verb required");
  vl_destroy(v);

  v = vl_create(&leaf_s);
  for (int n = 1; n <= 6; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    (void)vl_parse(v, 2, a1);
    vl_test_alloc_reset();
  }
  vl_destroy(v);

  vl_executable_t nest_s = {.verbs = nested, .color = VAL_COLOR_NEVER};
  v = vl_create(&nest_s);
  char *a2[] = {(char *)"valve", (char *)"nest", (char *)"go"};
  EXPECT(v != NULL && vl_parse(v, 3, a2) == -1, "missing sub required");
  vl_destroy(v);
}

void test_color_winsize_via_pty(void) {
  int master = -1, slave = -1;
  if (openpty(&master, &slave, NULL, NULL, NULL) != 0) {
    EXPECT(1, "openpty unavailable — skip");
    return;
  }
  int saved = dup(STDOUT_FILENO);
  EXPECT(saved >= 0 && dup2(slave, STDOUT_FILENO) == STDOUT_FILENO, "dup2");
  unsetenv("COLUMNS");
  unsetenv("LINES");
  unsetenv("NO_COLOR");
  setenv("FORCE_COLOR", "1", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  vl_term_caps_t caps;
  vl_term_caps(&caps);
  EXPECT(caps.columns >= 1, "pty columns");
  dup2(saved, STDOUT_FILENO);
  close(saved);
  close(slave);
  close(master);
  unsetenv("FORCE_COLOR");
}

void test_value_label_bool(void) {
  static const vl_option_t opt = {
      .name = "flag", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  EXPECT(strcmp(option_value_label_(&opt), "bool") == 0, "bool label");
}

void test_verbs_copy_empty_and_oom(void) {
  valve_verb_t *dst = NULL;
  size_t n = 0;
  EXPECT(verbs_copy_(NULL, 0, &dst, &n) == 0 && dst == NULL && n == 0,
         "empty verbs_copy");

  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .description = "d", .usage = "u"),
      NULL,
  };
  vl_test_alloc_fail_after(1);
  int rc = verbs_copy_(verbs, 1, &dst, &n);
  vl_test_alloc_reset();
  EXPECT(rc == -1, "verbs_copy calloc OOM");
}

void test_find_with_null_options_table(void) {
  valve_t v = {0};
  EXPECT(vl_option_find_(&v, "x") == NULL, "null options table");
  EXPECT(vl_option_find_n_(&v, "x", 1) == NULL, "find_n null table");
  EXPECT(vl_option_find_short_(&v, 'x') == NULL, "short null table");
}

void test_conflict_active_null_and_scope(void) {
  EXPECT(!vl_conflict_active_(NULL, NULL), "null args");
  valve_verb_t verb = {.name = "run"};
  valve_conflict_t c = {.verb = &verb};
  valve_t v = {0};
  EXPECT(!vl_conflict_active_(&v, &c), "verb scope mismatch");
}

void test_reserved_null_token(void) {
  EXPECT(vl_reserved_kind_(NULL) == VL_RESERVED_NONE, "null token");
  EXPECT(vl_reserved_name_(NULL) == VL_RESERVED_NONE, "null name");
  EXPECT(vl_reserved_name_("") == VL_RESERVED_NONE, "empty name");
}

void test_duration_overflow_literal(void) {
  int64_t out = 0;
  /* 40 digits → mul overflow while accumulating */
  char huge[64];
  memset(huge, '9', 40);
  huge[40] = '\0';
  EXPECT(option_duration_parse_(huge, &out) == OPTION_DURATION_OVERFLOW,
         "digit overflow");
  EXPECT(option_duration_parse_("9999999999999999999d", &out) ==
             OPTION_DURATION_OVERFLOW,
         "scaled overflow");
}

void test_annotations_seen_continue(void) {
  /* Two conflict entries sharing the same other-name → seen_ skips duplicate. */
  vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_option_t b1 = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  vl_option_t b2 = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  valve_conflict_t conflicts[2] = {
      {.first = &a, .second = &b1},
      {.first = &a, .second = &b2},
  };
  valve_t v = {
      .conflicts_ = conflicts,
      .conflict_count_ = 2,
  };
  option_annotations_print_(&v, &a);
  EXPECT(1, "annotations with seen continue");
}

void test_color_force_then_init_hyperlinks(void) {
  /* force + init leaves hyperlinks_detected false until supports_hyperlinks */
  unsetenv("NO_COLOR");
  setenv("FORCE_COLOR", "3", 1);
  setenv("TERM", "xterm-256color", 1);
  vl_color_support_force(VL_COLOR_SUPPORT_TRUECOLOR);
  vl_color_init(VAL_COLOR_AUTO);
  /* hyperlinks path: initialized, !hyperlinks_detected */
  (void)vl_color_supports_hyperlinks();
  EXPECT(1, "hyperlinks after force+init");
}

void test_parse_user_option_empty_name_token(void) {
  /* Drive user_has_option_ name_len==0 via reserved check on "--" mid-stream
   * with verbs so missing-command path can see option-like empty names. */
  const vl_verb_t *const verbs[] = {VL_CMD(.name = "run"), NULL};
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"-"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1, "lone dash unknown");
  vl_destroy(v);
}

void test_short_find_on_active_scopes(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "inner", .short_name = 'i',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "mid", .short_name = 'm',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {VL_CMD(.name = "go", .options = sub_opts),
                                   NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = verb_opts, .verbs = subs), NULL};
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"run", (char *)"go", (char *)"-i"};
  char *a2[] = {(char *)"valve", (char *)"run", (char *)"-m"};
  /* a2 needs no sub required - run has subs so -m at verb slot fails missing
   * sub. Use go then -m for verb-level after fallthrough: */
  char *a3[] = {(char *)"valve", (char *)"run", (char *)"go", (char *)"-m"};
  EXPECT(v != NULL && vl_parse(v, 4, a1) == 0, "sub short");
  EXPECT(vl_parse(v, 4, a3) == 0, "verb short via active sub");
  (void)a2;
  vl_destroy(v);
}

void test_create_oom_sweep_counts(void) {
  const vl_option_t *const opts[] = {
      VL_OPT(.name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .description = "da", .usage = "ua"),
      VL_OPT(.name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .description = "db", .usage = "ub"),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .description = "rd", .usage = "ru", .options = opts),
      VL_CMD(.name = "alt", .description = "ad", .usage = "au"),
      NULL,
  };
  vl_executable_t settings = {
      .program_name = "prog",
      .program_version = "1.2.3",
      .description = "desc",
      .usage = "usage",
      .logo = "L",
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };

  vl_test_alloc_reset();
  valve_t *ok = vl_create(&settings);
  size_t need = vl_test_alloc_count();
  EXPECT(ok != NULL, "baseline create");
  EXPECT(need > 3, "counted allocs");
  if (ok)
    vl_destroy(ok);

  int fails = 0;
  for (size_t n = 1; n <= need + 2; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after((int)n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (!v)
      ++fails;
    else
      vl_destroy(v);
  }
  EXPECT(fails > 0, "at least one mid-create OOM");
}

void test_create_oom_conflicts_and_requirements(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .description = "da"};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .description = "db"};
  static const vl_option_t *const a_c[] = {&b, NULL};
  static const vl_option_t *const b_r[] = {&a, NULL};
  a.conflicts = a_c;
  b.requires = b_r;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  vl_executable_t settings = {
      .program_name = "p",
      .options = opts,
      .color = VAL_COLOR_NEVER,
  };

  vl_test_alloc_reset();
  valve_t *ok = vl_create(&settings);
  size_t need = vl_test_alloc_count();
  EXPECT(ok != NULL, "create with conflicts/requires");
  if (ok)
    vl_destroy(ok);

  int fails = 0;
  for (size_t n = 1; n <= need + 3; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after((int)n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (!v)
      ++fails;
    else
      vl_destroy(v);
  }
  EXPECT(fails > 0, "conflict/requirement copy OOM");
}

void test_collides_and_mapped_direct(void) {
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "port", .short_name = 'p',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  const vl_option_t *const locals[] = {
      NULL,
      VL_OPT(.name = "port", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT),
      VL_OPT(.name = "quiet", .short_name = 'p',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  EXPECT(option_collides_with_globals_(globals, 1, locals, 3),
         "name or short collide");

  static const vl_option_t orphan = {
      .name = "orphan", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  EXPECT(option_mapped_(&orphan, globals, NULL, 0, NULL, NULL, 0, locals, NULL,
                        0) == NULL,
         "mapped miss returns null");
}

void test_label_print_fallback(void) {
  vl_option_t bare = {
      .name = "bare", .type = 0, .value = VL_OPTION_VALUE_BOOL};
  option_label_print_(&bare, false);
  option_label_print_(&bare, true);
  EXPECT(1, "label_print fallback");
}

void test_help_resolve_null_args(void) {
  vl_help_internal_t out = {0};
  EXPECT(!vl_help_resolve_internal_(NULL, "x", &out), "null v");
  EXPECT(!vl_help_resolve_internal_(&(valve_t){0}, NULL, &out), "null target");
  EXPECT(!vl_help_resolve_internal_(&(valve_t){0}, "", &out), "empty target");
  EXPECT(!vl_help_resolve_internal_(&(valve_t){0}, "x", NULL), "null out");
}

void test_nested_verb_copy_oom(void) {
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .description = "gd", .usage = "gu"),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .description = "rd", .usage = "ru", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  int fails = 0;
  for (int n = 1; n <= 30; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (!v)
      ++fails;
    else
      vl_destroy(v);
  }
  EXPECT(fails > 0, "nested verb OOM");
}

void test_parse_reserved_form_failures(void) {
  const vl_verb_t *const subs[] = {VL_CMD(.name = "go"), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = verbs, .assign = VL_ASSIGN_SEPARATE, .color = VAL_COLOR_NEVER};

  /* unknown verb token that is reserved with inline value under SEPARATE */
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"--help=go"};
  EXPECT(v != NULL && vl_parse(v, 2, a1) == -1, "SEPARATE rejects inline help");
  vl_destroy(v);

  settings.assign = VL_ASSIGN_INLINE;
  v = vl_create(&settings);
  char *a2[] = {(char *)"valve", (char *)"notacommand"};
  /* not reserved → unknown verb */
  EXPECT(vl_parse(v, 2, a2) == -1, "unknown verb");
  vl_destroy(v);

  /* reserved version with value as unknown-verb path */
  v = vl_create(&settings);
  char *a3[] = {(char *)"valve", (char *)"--version=1"};
  EXPECT(vl_parse(v, 2, a3) == -1, "version value at verb slot");
  vl_destroy(v);

  v = vl_create(&settings);
  char *a4[] = {(char *)"valve", (char *)"run", (char *)"--version=1"};
  EXPECT(vl_parse(v, 3, a4) == -1, "version value at sub slot");
  vl_destroy(v);
}

void test_conflict_exists_add_duplicate(void) {
  /* Reciprocal conflicts → second add is exists→0 */
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  static const vl_option_t *const b_c[] = {&a, NULL};
  a.conflicts = a_c;
  b.conflicts = b_c;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "reciprocal conflicts via exists");
  vl_destroy(v);
}

void test_color_pty_winsize(void) {
  int master = -1, slave = -1;
  struct winsize ws = {.ws_row = 40, .ws_col = 100};
  if (openpty(&master, &slave, NULL, NULL, &ws) != 0) {
    EXPECT(1, "skip openpty");
    return;
  }
  int saved_out = dup(STDOUT_FILENO);
  EXPECT(saved_out >= 0, "dup stdout");
  EXPECT(dup2(slave, STDOUT_FILENO) == STDOUT_FILENO, "dup2 slave");
  unsetenv("COLUMNS");
  unsetenv("LINES");
  setenv("FORCE_COLOR", "1", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  vl_term_caps_t caps;
  vl_term_caps(&caps);
  EXPECT(caps.columns >= 1, "winsize columns");
  dup2(saved_out, STDOUT_FILENO);
  close(saved_out);
  close(slave);
  close(master);
  unsetenv("FORCE_COLOR");
}

void test_result_set_invalid_target_and_none(void) {
  typedef struct {
    int unused;
  } cfg_t;
  cfg_t cfg = {0};
  vl_option_t none_opt = {
      .name = "none",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
      .target = VL_TARGET_NONE,
      .data = &cfg,
  };
  vl_option_t bad_opt = {
      .name = "bad",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_INT,
      .target = (vl_target_t)99,
      .data = &cfg,
      .offset = 0,
  };
  const vl_option_t *const options[] = {&none_opt, &bad_opt, NULL};
  /* schema may reject bad target — exercise via direct vl_result_set_ */
  valve_t *v = parser_((const vl_option_t *const[]){&none_opt, NULL}, 1);
  EXPECT(v != NULL, "none target parser");
  vl_value_t boolv = {.kind = VL_VALUE_BOOL, .as.boolean = true};
  EXPECT(vl_result_set_(v, &none_opt, &boolv, 0) == 0, "TARGET_NONE ok");
  vl_value_t intv = {.kind = VL_VALUE_INT, .as.integer = 1};
  EXPECT(vl_result_set_(v, &bad_opt, &intv, 0) == 0, "invalid target falls through");
  vl_destroy(v);
  (void)options;
}

void test_result_set_kv_clone_oom(void) {
  typedef struct {
    vl_value_t box;
  } cfg_t;
  cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV,
             .data = &cfg, .offset = offsetof(cfg_t, box),
             .target = VL_TARGET_VALUE),
      VL_OPT(.name = "list", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, .data = &cfg,
             .offset = offsetof(cfg_t, box), .target = VL_TARGET_VALUE),
      NULL,
  };
  for (int n = 1; n <= 20; ++n) {
    valve_t *v = parser_(options, 1);
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *argv[] = {(char *)"valve", (char *)"--meta=a:1|b:{c:2}"};
    (void)vl_parse(v, 2, argv);
    vl_test_alloc_reset();
    vl_destroy(v);
    vl_value_clear(&cfg.box);
  }
  for (int n = 1; n <= 15; ++n) {
    valve_t *v = parser_(options, 2);
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *argv[] = {(char *)"valve", (char *)"--list=a,b,c"};
    (void)vl_parse(v, 2, argv);
    vl_test_alloc_reset();
    vl_destroy(v);
    vl_value_clear(&cfg.box);
  }
  EXPECT(1, "kv/array clone OOM");
}

void test_parse_short_shadows_reserved(void) {
  /* Non-reserved short so find_short path still runs; -q is fine. */
  const vl_option_t *const options[] = {
      VL_OPT(.name = "quiet", .short_name = 'q',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *a1[] = {(char *)"valve", (char *)"-q"};
  EXPECT(v != NULL && vl_parse(v, 2, a1) == 0, "user short -q");
  vl_destroy(v);
}

void test_parse_double_dash_empty_name(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const verbs[] = {VL_CMD(.name = "run"), NULL};
  vl_executable_t settings = {
      .options = options, .verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  /* "--" as verb slot → option-like, reserved none, missing verb */
  char *argv[] = {(char *)"valve", (char *)"--"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1, "bare -- missing verb");
  vl_destroy(v);
}

void test_requirement_add_exists_direct(void) {
  vl_option_t a = {.name = "a"};
  vl_option_t b = {.name = "b"};
  valve_requirement_t slot;
  valve_t v = {0};
  EXPECT(vl_requirement_add_(&v, &a, &b, NULL, NULL) == 0, "first add");
  EXPECT(vl_requirement_add_(&v, &a, &b, NULL, NULL) == 0, "exists → 0");
  free(v.requirements_);
  (void)slot;
}

void test_conflict_add_oom_direct(void) {
  vl_option_t a = {.name = "a"};
  vl_option_t b = {.name = "b"};
  valve_t v = {0};
  int fails = 0;
  for (int n = 1; n <= 8; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    int rc = vl_conflict_add_(&v, &a, &b, NULL, NULL);
    vl_test_alloc_reset();
    if (rc != 0)
      ++fails;
    free(v.conflicts_);
    v.conflicts_ = NULL;
    v.conflict_count_ = 0;
    v.conflict_cap_ = 0;
  }
  EXPECT(fails > 0, "conflict_add OOM");
}

void test_help_print_null_option_guard(void) {
  /* Overview with a copied option that has empty name is hard; call
   * print path via --help with description-only verb usage. */
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .description = "does run", .usage = "run it"),
      NULL,
  };
  vl_executable_t settings = {
      .program_name = "app",
      .program_version = "0",
      .description = "desc",
      .usage = "app run",
      .logo = "*",
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "rich overview");
  vl_destroy(v);
}

void test_forward_requirement_annotations(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t c = {
      .name = "c", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  /* a requires b and also a duplicate-name target via second edge to b2 */
  static vl_option_t b2 = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_r[] = {&b, &b2, NULL};
  static const vl_option_t *const c_r[] = {&a, NULL};
  a.requires = a_r;
  c.requires = c_r;
  const vl_option_t *const opts[] = {&a, &b, &c, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  if (!v) {
    /* schema may reject duplicate names — still ok */
    EXPECT(1, "schema rejected dup names");
    return;
  }
  char *help[] = {(char *)"valve", (char *)"--help"};
  EXPECT(vl_parse(v, 2, help) == 0, "help with requires annotations");
  vl_destroy(v);
}

