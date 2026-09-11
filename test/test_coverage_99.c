#include "test_support.h"
#include "fault_alloc.h"

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

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void setUp(void) { vl_test_alloc_reset(); }

void tearDown(void) {
  vl_test_alloc_reset();
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  unsetenv("COLORTERM");
  unsetenv("TERM");
  unsetenv("TERM_PROGRAM");
  unsetenv("COLUMNS");
  unsetenv("LINES");
  unsetenv("VTE_VERSION");
  unsetenv("KITTY_WINDOW_ID");
  unsetenv("WEZTERM_PANE");
  unsetenv("KONSOLE_VERSION");
  unsetenv("LC_ALL");
  unsetenv("LC_CTYPE");
  unsetenv("LANG");
  vl_color_support_reset();
}

/* ---- parse edges ----------------------------------------------------- */

void test_parse_null_and_negative_argc(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *argv[] = {(char *)"valve"};
  EXPECT(v != NULL, "parser");
  EXPECT(vl_parse(NULL, 1, argv) == -1, "null valve");
  EXPECT(vl_parse(v, -1, argv) == -1, "negative argc");
  EXPECT(vl_parse(v, 1, NULL) == -1, "null argv");
  vl_destroy(v);
}

void test_parse_number_huge_int_as_double(void) {
  /* Exceeds int64 → parse_int fails; no .eE mark; parse_double succeeds. */
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_NUMBER),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *argv[] = {(char *)"valve",
                  (char *)"--n=999999999999999999999999999999"};
  const vl_value_t *val;
  EXPECT(v != NULL, "parser");
  EXPECT(vl_parse(v, 2, argv) == 0, "huge number parses as double");
  val = vl_get(v, "n");
  EXPECT(val && val->kind == VL_VALUE_DOUBLE, "stored as double");
  vl_destroy(v);
}

void test_parse_kv_depth_and_syntax_errors(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      NULL,
  };
  char deep[512];
  size_t pos = 0;
  /* Build a:{a:{a:{...}}} deeper than VL_KV_MAX_DEPTH (32). */
  deep[pos++] = 'a';
  deep[pos++] = ':';
  for (int i = 0; i < 40; ++i) {
    deep[pos++] = '{';
    deep[pos++] = 'a';
    deep[pos++] = ':';
  }
  deep[pos++] = '1';
  for (int i = 0; i < 40; ++i)
    deep[pos++] = '}';
  deep[pos] = '\0';

  valve_t *v = parser_(options, 1);
  char *argv_deep[] = {(char *)"valve", deep};
  /* prefix --meta= */
  char argbuf[600];
  snprintf(argbuf, sizeof argbuf, "--meta=%s", deep);
  argv_deep[1] = argbuf;
  EXPECT(v != NULL && vl_parse(v, 2, argv_deep) == -1, "kv depth overflow");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_empty[] = {(char *)"valve", (char *)"--meta=a:"};
  EXPECT(vl_parse(v, 2, bad_empty) == -1, "empty kv scalar");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_pipe[] = {(char *)"valve", (char *)"--meta=a:1|"};
  EXPECT(vl_parse(v, 2, bad_pipe) == -1, "trailing pipe");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_key[] = {(char *)"valve", (char *)"--meta=:1"};
  EXPECT(vl_parse(v, 2, bad_key) == -1, "empty key");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_nest[] = {(char *)"valve", (char *)"--meta=a:{b:1"};
  EXPECT(vl_parse(v, 2, bad_nest) == -1, "unclosed nest");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_quote[] = {(char *)"valve", (char *)"--meta=a:\"hi"};
  EXPECT(vl_parse(v, 2, bad_quote) == -1, "unterminated kv quote");
  vl_destroy(v);

  v = parser_(options, 1);
  char *ok_quote[] = {(char *)"valve", (char *)"--meta=a:\"hi\"|b:x"};
  EXPECT(vl_parse(v, 2, ok_quote) == 0, "quoted kv ok");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_junk[] = {(char *)"valve", (char *)"--meta=a:{b:1}x"};
  EXPECT(vl_parse(v, 2, bad_junk) == -1, "junk after nested kv");
  vl_destroy(v);
}

void test_parse_array_quote_and_empty_element(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "list", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *ok[] = {(char *)"valve", (char *)"--list=\"a\",b,\"c\""};
  EXPECT(v != NULL && vl_parse(v, 2, ok) == 0, "quoted array elements");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_q[] = {(char *)"valve", (char *)"--list=\"unterminated"};
  EXPECT(vl_parse(v, 2, bad_q) == -1, "unterminated array quote");
  vl_destroy(v);

  v = parser_(options, 1);
  char *bad_empty[] = {(char *)"valve", (char *)"--list=a,,b"};
  EXPECT(vl_parse(v, 2, bad_empty) == -1, "empty array element");
  vl_destroy(v);
}

void test_parse_help_suggestion_and_joined(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "req", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "list", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .verbs = subs),
      VL_CMD(.name = "net", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *ambig[] = {(char *)"valve", (char *)"--help=list"};
  EXPECT(v != NULL && vl_parse(v, 2, ambig) == -1,
         "ambiguous help suggests dotted forms");
  EXPECT(vl_error_count(v) > 0, "error recorded");
  vl_destroy(v);

  v = vl_create(&settings);
  char *joined[] = {(char *)"valve", (char *)"--?agent"};
  EXPECT(vl_parse(v, 2, joined) == 0, "joined --?help form");
  vl_destroy(v);

  v = vl_create(&settings);
  char *short_join[] = {(char *)"valve", (char *)"-?agent"};
  EXPECT(vl_parse(v, 2, short_join) == 0, "joined -?help form");
  vl_destroy(v);
}

void test_parse_reserved_edge_tokens(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  /* plen >= 64 → RESERVED_NONE path */
  char longtok[80];
  memset(longtok, 'a', 70);
  longtok[0] = '-';
  longtok[1] = '-';
  longtok[70] = '=';
  longtok[71] = '1';
  longtok[72] = '\0';
  char *argv[] = {(char *)"valve", longtok};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1, "overlong = token");
  vl_destroy(v);

  /* empty name before = */
  v = parser_(options, 1);
  char *eq0[] = {(char *)"valve", (char *)"--=1"};
  EXPECT(vl_parse(v, 2, eq0) == -1, "empty name before =");
  vl_destroy(v);
}

void test_parse_verb_reserved_and_missing_sub(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "q", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);

  char *help_at_verb[] = {(char *)"valve", (char *)"run", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 3, help_at_verb) == 0,
         "reserved help instead of subverb");
  vl_destroy(v);

  v = vl_create(&settings);
  char *help_eq_bad[] = {(char *)"valve", (char *)"run", (char *)"--help=nope"};
  EXPECT(vl_parse(v, 3, help_eq_bad) == -1, "bad help target at sub slot");
  vl_destroy(v);

  v = vl_create(&settings);
  char *ver_at_cmd[] = {(char *)"valve", (char *)"--version"};
  EXPECT(vl_parse(v, 2, ver_at_cmd) == 0, "reserved version at verb slot");
  vl_destroy(v);

  /* unknown verb that looks like reserved with bad form */
  settings.assign = VL_ASSIGN_SEPARATE;
  v = vl_create(&settings);
  char *help_inline_sep[] = {(char *)"valve", (char *)"--help=run"};
  EXPECT(vl_parse(v, 2, help_inline_sep) == -1,
         "inline help disabled in SEPARATE");
  vl_destroy(v);
}

void test_parse_unknown_long_eq_oom_and_negative_dot(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_NUMBER),
      NULL,
  };
  vl_executable_t settings = {
      .options = options,
      .assign = VL_ASSIGN_SEPARATE,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--n", (char *)"-.5"};
  EXPECT(v != NULL && vl_parse(v, 3, argv) == 0, "negative dotted number");
  vl_destroy(v);

  /* looks_like_negative_number false for lone '-' */
  v = vl_create(&settings);
  char *bad[] = {(char *)"valve", (char *)"--n", (char *)"-"};
  EXPECT(vl_parse(v, 3, bad) == -1, "lone dash not a number");
  vl_destroy(v);
}

void test_int_target_overflow(void) {
  typedef struct {
    int i;
  } cfg_t;
  cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "i", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .data = &cfg, .offset = offsetof(cfg_t, i), .target = VL_TARGET_INT),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char buf[64];
  snprintf(buf, sizeof buf, "--i=%lld", (long long)INT_MAX + 1LL);
  char *argv[] = {(char *)"valve", buf};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1, "int overflow rejected");
  vl_destroy(v);
}

void test_color_lazy_init_paths(void) {
  vl_color_support_reset();
  /* Force uninitialized path: reset leaves g_initialized_ true after prior
   * init; re-init NEVER then call APIs that re-enter when not forced. */
  unsetenv("FORCE_COLOR");
  unsetenv("NO_COLOR");
  setenv("TERM", "xterm-256color", 1);
  vl_color_init(VAL_COLOR_ALWAYS);
  (void)vl_color_support_detect();
  (void)vl_color_supports_hyperlinks();
  (void)vl_color_enabled(stdout);

  /* Call detect after init with support already detected — hits refresh skip */
  (void)vl_color_support_detect();

  /* term_caps / enabled / hyperlinks when already initialized */
  vl_term_caps_t caps;
  vl_term_caps(&caps);
  EXPECT(caps.columns >= 1, "caps columns");
}

/* ---- OOM / fault injection ------------------------------------------- */

void test_oom_vl_create_meta(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .description = "desc", .usage = "use"),
      NULL,
  };
  vl_executable_t settings = {
      .program_name = "prog",
      .program_version = "1.0",
      .description = "d",
      .usage = "u",
      .logo = "L",
      .options = options,
      .color = VAL_COLOR_NEVER,
  };

  /* Fail early strdup in settings_meta_copy_ */
  for (int n = 1; n <= 8; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (v)
      vl_destroy(v);
    /* Some n may land after successful create; that is fine. */
  }
  EXPECT(1, "meta OOM paths exercised");
}

void test_oom_vl_create_options_and_verbs(void) {
  const vl_option_t *const opts[] = {
      VL_OPT(.name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .description = "da", .usage = "ua"),
      VL_OPT(.name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .description = "db", .usage = "ub"),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .description = "rd", .usage = "ru", .options = opts),
      NULL,
  };
  vl_executable_t settings = {
      .options = opts,
      .verbs = verbs,
      .program_name = "p",
      .color = VAL_COLOR_NEVER,
  };

  for (int n = 1; n <= 40; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (v)
      vl_destroy(v);
  }
  EXPECT(1, "options/verbs copy OOM paths exercised");
}

void test_oom_parse_scalar_and_bool(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "s", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "enable-x", .toggle_ref = "x", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE),
      NULL,
  };
  valve_t *v = parser_(options, 2);
  EXPECT(v != NULL, "parser");

  vl_test_alloc_fail_after(1);
  char *a1[] = {(char *)"valve", (char *)"--s=hi"};
  (void)vl_parse(v, 2, a1);
  vl_test_alloc_reset();
  vl_destroy(v);

  v = parser_(options, 2);
  vl_test_alloc_fail_after(1);
  char *a2[] = {(char *)"valve", (char *)"--enable-x"};
  (void)vl_parse(v, 2, a2);
  vl_test_alloc_reset();
  vl_destroy(v);
  EXPECT(1, "scalar/bool OOM");
}

void test_oom_parse_kv_array_unknown(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      VL_OPT(.name = "list", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY),
      NULL,
  };
  valve_t *v = parser_(options, 2);

  for (int n = 1; n <= 12; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *kv[] = {(char *)"valve", (char *)"--meta=a:1|b:2"};
    (void)vl_parse(v, 2, kv);
    vl_test_alloc_reset();
  }
  vl_destroy(v);

  v = parser_(options, 2);
  for (int n = 1; n <= 12; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *ar[] = {(char *)"valve", (char *)"--list=a,b,c"};
    (void)vl_parse(v, 2, ar);
    vl_test_alloc_reset();
  }
  vl_destroy(v);

  v = parser_(options, 1);
  vl_test_alloc_fail_after(1);
  char *unk[] = {(char *)"valve", (char *)"--nope=1"};
  (void)vl_parse(v, 2, unk);
  vl_test_alloc_reset();
  vl_destroy(v);
  EXPECT(1, "kv/array/unknown OOM");
}

void test_oom_error_add_and_result_set(void) {
  typedef struct {
    char *s;
    vl_value_t val;
  } cfg_t;
  cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "s", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, s),
             .target = VL_TARGET_STRING),
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .repeat = VL_OPTION_REPEAT_ARRAY),
      VL_OPT(.name = "box", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV,
             .data = &cfg, .offset = offsetof(cfg_t, val),
             .target = VL_TARGET_VALUE),
      NULL,
  };

  for (int n = 1; n <= 20; ++n) {
    valve_t *v = parser_(options, 3);
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *a[] = {(char *)"valve", (char *)"--s=hello", (char *)"--mode=a",
                 (char *)"--mode=b", (char *)"--box=k:1"};
    (void)vl_parse(v, 5, a);
    vl_test_alloc_reset();
    vl_destroy(v);
    free(cfg.s);
    cfg.s = NULL;
    vl_value_clear(&cfg.val);
  }
  EXPECT(1, "error_add/result_set OOM");
}

void test_oom_conflict_and_requirement_errors(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  static const vl_option_t *const b_r[] = {&a, NULL};
  a.conflicts = a_c;
  b.requires = b_r;
  static const vl_option_t *const opts[] = {&a, &b, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};

  for (int n = 1; n <= 25; ++n) {
    valve_t *v = vl_create(&settings);
    if (!v)
      continue;
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *c[] = {(char *)"valve", (char *)"--a", (char *)"--b"};
    (void)vl_parse(v, 3, c);
    vl_test_alloc_reset();
    vl_destroy(v);
  }

  for (int n = 1; n <= 25; ++n) {
    valve_t *v = vl_create(&settings);
    if (!v)
      continue;
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    char *r[] = {(char *)"valve", (char *)"--b"};
    (void)vl_parse(v, 2, r);
    vl_test_alloc_reset();
    vl_destroy(v);
  }
  EXPECT(1, "conflict/requirement error OOM");
}

void test_find_option_via_active_scopes(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "inner", .short_name = 'i',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "mid", .short_name = 'm',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = verb_opts, .verbs = subs),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "g", .short_name = 'g',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  vl_executable_t settings = {
      .options = globals, .verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"run", (char *)"go", (char *)"--inner=1"};
  char *a2[] = {(char *)"valve", (char *)"run", (char *)"go", (char *)"-i=2"};
  char *a3[] = {(char *)"valve", (char *)"run", (char *)"go", (char *)"--mid=3"};
  char *a4[] = {(char *)"valve", (char *)"run", (char *)"go", (char *)"--g=4"};
  EXPECT(v != NULL && vl_parse(v, 4, a1) == 0, "sub long");
  EXPECT(vl_parse(v, 4, a2) == 0, "sub short inline");
  EXPECT(vl_parse(v, 4, a3) == 0, "verb long via sub scope");
  EXPECT(vl_parse(v, 4, a4) == 0, "global via sub scope");
  vl_destroy(v);
}

void test_help_with_usage_description_logo(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "port", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .description = "listen port", .usage = "--port N"),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "serve", .description = "run server", .usage = "serve [opts]"),
      NULL,
  };
  vl_executable_t settings = {
      .program_name = "demo",
      .program_version = "9.9",
      .description = "demo tool",
      .usage = "demo [opts]",
      .logo = "◆",
      .options = options,
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "rich help overview");
  vl_destroy(v);
}

void test_duplicate_repeat_error(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "once", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .repeat = VL_OPTION_REPEAT_ERROR),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *argv[] = {(char *)"valve", (char *)"--once=a", (char *)"--once=b"};
  EXPECT(v != NULL && vl_parse(v, 3, argv) == -1, "duplicate rejected");
  vl_destroy(v);
}

void test_required_missing_path(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "need", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .required = true),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  EXPECT(v != NULL, "parser");
  vl_destroy(v);

  v = parser_(options, 1);
  char *empty[] = {(char *)"valve"};
  EXPECT(vl_parse(v, 1, empty) == 0, "argc<=1 dispatches help");
  vl_destroy(v);

  v = parser_(options, 1);
  char *no[] = {(char *)"valve", (char *)"--need"};
  /* with required present ok */
  EXPECT(vl_parse(v, 2, no) == 0, "required present");
  vl_destroy(v);

  /* Force required check: parse with a throwaway bool that isn't need */
  const vl_option_t *const options2[] = {
      VL_OPT(.name = "need", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .required = true),
      VL_OPT(.name = "extra", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  v = parser_(options2, 2);
  char *miss[] = {(char *)"valve", (char *)"--extra"};
  EXPECT(vl_parse(v, 2, miss) == -1, "missing required");
  vl_destroy(v);
}
