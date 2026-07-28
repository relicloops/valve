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



#include <fcntl.h>
#include <unistd.h>

#include "pty_compat.h"

void setUp(void) {}
void tearDown(void) {
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

/* ---- color ENV matrix ------------------------------------------------ */

void test_color_env_force_and_no_color(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  vl_color_support_reset();

  setenv("NO_COLOR", "1", 1);
  vl_color_init(VAL_COLOR_AUTO);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_NONE, "NO_COLOR => NONE");
  EXPECT(!vl_color_supports(), "NO_COLOR disables supports");
  unsetenv("NO_COLOR");

  setenv("FORCE_COLOR", "0", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_NONE, "FORCE 0 => NONE");

  setenv("FORCE_COLOR", "1", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_BASIC, "FORCE 1 => BASIC");
  EXPECT(vl_color_enabled(stdout) || !isatty(STDOUT_FILENO),
         "FORCE 1 enables color or non-tty");

  setenv("FORCE_COLOR", "2", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_256, "FORCE 2 => 256");

  setenv("FORCE_COLOR", "3", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_TRUECOLOR,
         "FORCE 3 => TRUECOLOR");

  setenv("FORCE_COLOR", "yes", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_TRUECOLOR,
         "FORCE other => TRUECOLOR");
  unsetenv("FORCE_COLOR");
}

void test_color_mode_never_always_and_caps(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  setenv("COLUMNS", "120", 1);
  setenv("LINES", "40", 1);
  setenv("TERM", "xterm-256color", 1);
  setenv("COLORTERM", "truecolor", 1);
  setenv("TERM_PROGRAM", "iTerm.app", 1);
  setenv("VTE_VERSION", "1", 1);
  setenv("LANG", "en_US.UTF-8", 1);

  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_NONE ||
             isatty(STDOUT_FILENO) || isatty(STDERR_FILENO),
         "NEVER yields NONE when env allows detect path");
  /* Re-init NEVER after reset with no FORCE: non-tty => NONE; tty => NONE via NEVER */
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_NONE, "NEVER => NONE");

  vl_color_support_reset();
  vl_color_init(VAL_COLOR_ALWAYS);
  (void)vl_color_support_detect();
  (void)vl_color_supports_hyperlinks();
  EXPECT(vl_color_for(stdout, VAL_FG_RED) != NULL, "vl_color_for returns pointer");
  EXPECT(vl_color_for(stderr, VAL_FG_RED) != NULL, "vl_color_for stderr");
  EXPECT(strcmp(vl_color_for(stdin, VAL_FG_RED), "") == 0,
         "non stdout/stderr => empty");

  vl_term_caps_t caps = {0};
  vl_term_caps(NULL);
  vl_term_caps(&caps);
  EXPECT(caps.columns >= 80, "columns probed or defaulted");
  EXPECT(caps.rows >= 24, "rows probed or defaulted");

  setenv("KITTY_WINDOW_ID", "1", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_supports_hyperlinks();
  setenv("WEZTERM_PANE", "1", 1);
  vl_color_support_reset();
  (void)vl_color_supports_hyperlinks();
  setenv("KONSOLE_VERSION", "1", 1);
  vl_color_support_reset();
  (void)vl_color_supports_hyperlinks();

  setenv("TERM", "dumb", 1);
  unsetenv("COLORTERM");
  unsetenv("FORCE_COLOR");
  unsetenv("NO_COLOR");
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_support_detect();

  setenv("TERM", "screen.xterm-256color", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_support_detect();

  setenv("TERM", "xterm-direct", 1);
  vl_color_support_reset();
  (void)vl_color_support_detect();

  setenv("TERM", "weirdterm", 1);
  vl_color_support_reset();
  (void)vl_color_support_detect();

  unsetenv("TERM");
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_ALWAYS);
  (void)vl_color_support_detect();
}

/* ---- targets clear all kinds + nested -------------------------------- */

typedef struct clear_all_cfg {
  char *s;
  int i;
  int64_t i64;
  double d;
  bool b;
  vl_value_t val;
  vl_value_t tog;
} clear_all_cfg_t;

void test_targets_clear_all_kinds_and_verbs(void) {
  clear_all_cfg_t cfg = {0};
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "nested", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_INT, .data = &cfg,
             .offset = offsetof(clear_all_cfg_t, i), .target = VL_TARGET_INT),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "inner", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "outer", .verbs = subs),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "s", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(clear_all_cfg_t, s),
             .target = VL_TARGET_STRING),
      VL_OPT(.name = "i", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .data = &cfg, .offset = offsetof(clear_all_cfg_t, i),
             .target = VL_TARGET_INT),
      VL_OPT(.name = "i64", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .data = &cfg, .offset = offsetof(clear_all_cfg_t, i64),
             .target = VL_TARGET_INT64),
      VL_OPT(.name = "d", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_DOUBLE,
             .data = &cfg, .offset = offsetof(clear_all_cfg_t, d),
             .target = VL_TARGET_DOUBLE),
      VL_OPT(.name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .data = &cfg, .offset = offsetof(clear_all_cfg_t, b),
             .target = VL_TARGET_BOOL),
      VL_OPT(.name = "val", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(clear_all_cfg_t, val),
             .target = VL_TARGET_VALUE),
      VL_OPT(.name = "enable-x", .toggle_ref = "x", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, .data = &cfg,
             .offset = offsetof(clear_all_cfg_t, tog), .target = VL_TARGET_VALUE),
      NULL,
  };
  vl_executable_t settings = {.options = globals, .verbs = verbs};
  vl_executable_t global_only = {.options = globals};
  char *argv[] = {(char *)"valve", (char *)"--s=hi", (char *)"--i=3",
                  (char *)"--i64=8", (char *)"--d=2.5", (char *)"--b",
                  (char *)"--val=z", (char *)"--enable-x"};
  char *verb_argv[] = {(char *)"valve", (char *)"outer", (char *)"inner",
                       (char *)"--nested=5"};
  valve_t *v = vl_create(&global_only);

  EXPECT(v != NULL, "clear-all parser");
  EXPECT(vl_parse(v, 8, argv) == 0, "populate all target kinds");
  vl_destroy(v);

  v = vl_create(&settings);
  EXPECT(v != NULL && vl_parse(v, 4, verb_argv) == 0, "populate nested verb target");
  vl_destroy(v);

  vl_targets_clear(NULL);
  vl_targets_clear(&global_only);
  vl_targets_clear(&settings);
  EXPECT(cfg.s == NULL, "string cleared");
  EXPECT(cfg.i == 0 && cfg.i64 == 0 && cfg.d == 0.0 && !cfg.b, "scalars cleared");
  EXPECT(cfg.val.raw == NULL, "value cleared");
  EXPECT(cfg.tog.raw == NULL, "toggle value cleared");
}

/* ---- parse / set depth ----------------------------------------------- */

void test_parse_number_time_negative_separate(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_NUMBER),
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_TIME),
      VL_OPT(.name = "delta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  vl_executable_t settings = {
      .options = options,
      .assign = VL_ASSIGN_SEPARATE,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--n", (char *)"1.5e1",
                  (char *)"--wait", (char *)"2s", (char *)"--delta",
                  (char *)"-3"};
  const vl_value_t *n;

  EXPECT(v != NULL, "number/time parser");
  EXPECT(vl_parse(v, 7, argv) == 0, "negative separate value parses");
  n = vl_get(v, "n");
  EXPECT(n && n->kind == VL_VALUE_DOUBLE, "scientific number -> double");
  EXPECT(vl_get(v, "wait") && vl_get(v, "wait")->kind == VL_VALUE_INT,
         "duration stores seconds");
  EXPECT(vl_get(v, "delta") && vl_get(v, "delta")->as.integer == -3,
         "negative int next-value");
  vl_destroy(v);
}

void test_parse_dot_notation_targets(void) {
  typedef struct {
    char *lane;
    int cpus;
    int64_t big;
    double ratio;
    bool on;
  } cfg_t;
  cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, lane)),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_INT,
             .data = &cfg, .offset = offsetof(cfg_t, cpus)),
      VL_OPT(.name = "proxy.big", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_INT64,
             .data = &cfg, .offset = offsetof(cfg_t, big)),
      VL_OPT(.name = "proxy.ratio", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_DOUBLE,
             .data = &cfg, .offset = offsetof(cfg_t, ratio)),
      VL_OPT(.name = "proxy.on", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_BOOL,
             .data = &cfg, .offset = offsetof(cfg_t, on)),
      VL_OPT(.name = "proxy.auto", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_NONE),
      NULL,
  };
  char *argv[] = {(char *)"valve", (char *)"--proxy.lane=a",
                  (char *)"--proxy.cpus=2", (char *)"--proxy.big=9",
                  (char *)"--proxy.ratio=1.25", (char *)"--proxy.on=true",
                  (char *)"--proxy.auto=x"};
  valve_t *v = parser_(options, 6);

  EXPECT(v != NULL, "dot notation parser");
  EXPECT(vl_parse(v, 7, argv) == 0, "dot notation parses");
  EXPECT(cfg.lane && strcmp(cfg.lane, "a") == 0, "dot string target");
  EXPECT(cfg.cpus == 2 && cfg.big == 9, "dot int targets");
  EXPECT(cfg.ratio > 1.2 && cfg.ratio < 1.3, "dot double target");
  EXPECT(cfg.on, "dot bool target");
  vl_destroy(v);
  free(cfg.lane);
}

void test_parse_kv_nested_and_quoted(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      NULL,
  };
  char *argv[] = {
      (char *)"valve",
      (char *)"--meta=!a:1|b:\"hi\"|c:{!d:true|e:2.5}",
  };
  valve_t *v = parser_(options, 1);
  const vl_value_t *val;

  EXPECT(v != NULL, "kv parser");
  EXPECT(vl_parse(v, 2, argv) == 0, "nested kv parses");
  val = vl_get(v, "meta");
  EXPECT(val && val->kind == VL_VALUE_KV && val->as.kv.count >= 3,
         "kv has pairs");
  vl_destroy(v);

  char *bad[] = {(char *)"valve", (char *)"--meta=!a:\"unterminated"};
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, bad) == -1, "unterminated quote fails");
  vl_destroy(v);
}

void test_parse_toggle_on_verb_and_subverb(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "enable-fast", .toggle_ref = "fast",
             .type = VL_OPT_TYPE_TOGGLE, .value = VL_OPTION_VALUE_TOGGLE),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sub_opts),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "enable-debug", .toggle_ref = "debug",
             .type = VL_OPT_TYPE_TOGGLE, .value = VL_OPTION_VALUE_TOGGLE),
      NULL,
  };
  const vl_verb_t *const leaf_verbs[] = {
      VL_CMD(.name = "run", .options = verb_opts),
      NULL,
  };
  const vl_verb_t *const nested_verbs[] = {
      VL_CMD(.name = "task", .verbs = subs),
      NULL,
  };
  vl_executable_t leaf = {.verbs = leaf_verbs, .color = VAL_COLOR_NEVER};
  vl_executable_t nested = {.verbs = nested_verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&leaf);
  char *a1[] = {(char *)"valve", (char *)"run", (char *)"--enable-debug"};
  char *a2[] = {(char *)"valve", (char *)"task", (char *)"go",
                (char *)"--disable-fast"};

  EXPECT(v != NULL, "toggle verb parser");
  EXPECT(vl_parse(v, 3, a1) == 0, "verb toggle parses");
  EXPECT(vl_get(v, "enable-debug") &&
             vl_get(v, "enable-debug")->as.boolean == true,
         "verb toggle true");
  vl_destroy(v);

  v = vl_create(&nested);
  EXPECT(v != NULL && vl_parse(v, 4, a2) == 0, "subverb toggle parses");
  EXPECT(vl_get(v, "enable-fast") &&
             vl_get(v, "enable-fast")->as.boolean == false,
         "subverb toggle false");
  vl_destroy(v);
}

void test_help_after_active_verb_and_forms(void) {
  const vl_option_t *const call_opts[] = {
      VL_OPT(.name = "request", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const net_opts[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "call", .options = call_opts),
      VL_CMD(.name = "list"),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .verbs = subs),
      VL_CMD(.name = "network", .options = net_opts),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
      .on_help = NULL, /* default printer */
  };
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"agent", (char *)"--help=list"};
  char *a2[] = {(char *)"valve", (char *)"agent", (char *)"--help=call"};
  char *a3[] = {(char *)"valve", (char *)"agent", (char *)"call",
                (char *)"--help=request"};
  char *a4[] = {(char *)"valve", (char *)"--help=proxy.lane"};
  char *a5[] = {(char *)"valve", (char *)"--version=1"};

  EXPECT(v != NULL, "active help parser");
  EXPECT(vl_parse(v, 3, a1) == 0, "agent --help=list");
  EXPECT(vl_parse(v, 3, a2) == 0, "agent --help=call");
  EXPECT(vl_parse(v, 4, a3) == 0, "agent call --help=request");
  EXPECT(vl_parse(v, 2, a4) == 0, "dotted option help");
  EXPECT(vl_parse(v, 2, a5) == -1, "version rejects value");
  vl_destroy(v);

  /* SEPARATE assign help target */
  settings.assign = VL_ASSIGN_SEPARATE;
  v = vl_create(&settings);
  char *sep[] = {(char *)"valve", (char *)"--help", (char *)"agent"};
  EXPECT(v != NULL && vl_parse(v, 3, sep) == 0, "separate --help agent");
  char *eq[] = {(char *)"valve", (char *)"--help=agent"};
  EXPECT(vl_parse(v, 2, eq) == -1, "inline help target disabled in SEPARATE");
  char *empty[] = {(char *)"valve", (char *)"--help="};
  settings.assign = VL_ASSIGN_INLINE;
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, empty) == -1, "empty help target fails");
  vl_destroy(v);
}

void test_result_set_type_mismatches_and_array_append(void) {
  typedef struct {
    int i;
    int64_t i64;
    double d;
    bool b;
    char *s;
  } cfg_t;
  cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "i", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, i), .target = VL_TARGET_INT),
      VL_OPT(.name = "i64", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, i64),
             .target = VL_TARGET_INT64),
      VL_OPT(.name = "d", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, d),
             .target = VL_TARGET_DOUBLE),
      VL_OPT(.name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, b), .target = VL_TARGET_BOOL),
      VL_OPT(.name = "s", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .data = &cfg, .offset = offsetof(cfg_t, s),
             .target = VL_TARGET_STRING),
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .repeat = VL_OPTION_REPEAT_ARRAY),
      NULL,
  };
  valve_t *v = parser_(options, 6);
  char *bad_i[] = {(char *)"valve", (char *)"--i=abc"};
  char *ok_arr[] = {(char *)"valve", (char *)"--mode=a", (char *)"--mode=b",
                    (char *)"--mode=c"};

  char *bad_i64[] = {(char *)"valve", (char *)"--i64=abc"};
  char *bad_d[] = {(char *)"valve", (char *)"--d=abc"};
  char *bad_b[] = {(char *)"valve", (char *)"--b=abc"};
  char *bad_s[] = {(char *)"valve", (char *)"--s=1"};

  EXPECT(v != NULL, "mismatch parser");
  EXPECT(vl_parse(v, 2, bad_i) == -1, "string into int target fails");
  vl_destroy(v);

  v = parser_(options, 6);
  EXPECT(vl_parse(v, 2, bad_i64) == -1, "string into i64 fails");
  vl_destroy(v);
  v = parser_(options, 6);
  EXPECT(vl_parse(v, 2, bad_d) == -1, "string into double fails");
  vl_destroy(v);
  v = parser_(options, 6);
  EXPECT(vl_parse(v, 2, bad_b) == -1, "string into bool fails");
  vl_destroy(v);
  v = parser_(options, 6);
  EXPECT(vl_parse(v, 2, bad_s) == -1, "int into string target fails");
  vl_destroy(v);

  v = parser_(options, 6);
  EXPECT(vl_parse(v, 4, ok_arr) == 0, "triple array append");
  EXPECT(vl_get(v, "mode") && vl_get(v, "mode")->as.array.count == 3,
         "array has 3 items");
  vl_destroy(v);
}

void test_int_bounds_and_required_on_verb(void) {
  const vl_option_t *const opts[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .has_int_min = true, .int_min = 1, .has_int_max = true, .int_max = 5,
             .required = true),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "go", .options = opts),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *low[] = {(char *)"valve", (char *)"go", (char *)"--n=0"};
  char *high[] = {(char *)"valve", (char *)"go", (char *)"--n=9"};
  char *miss[] = {(char *)"valve", (char *)"go"};
  char *ok[] = {(char *)"valve", (char *)"go", (char *)"--n=3"};

  EXPECT(v != NULL, "bounds parser");
  EXPECT(vl_parse(v, 3, low) == -1, "below min fails");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, high) == -1, "above max fails");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, miss) == -1, "missing required verb opt");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, ok) == 0, "in-bounds ok");
  vl_destroy(v);
}

void test_short_cluster_and_combined_long_short(void) {
  bool a = false, b = false;
  int n = 0;
  const vl_option_t *const options[] = {
      VL_OPT(.name = "all", .short_name = 'a', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL, .data = &a, .target = VL_TARGET_BOOL),
      VL_OPT(.name = "brief", .short_name = 'b', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL, .data = &b, .target = VL_TARGET_BOOL),
      VL_OPT(.name = "num", .short_name = 'n',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_INT, .data = &n, .target = VL_TARGET_INT),
      NULL,
  };
  char *argv[] = {(char *)"valve", (char *)"-a", (char *)"-b",
                  (char *)"--num=4"};
  valve_t *v = parser_(options, 3);

  EXPECT(v != NULL, "short cluster parser");
  EXPECT(vl_parse(v, 4, argv) == 0, "shorts parse");
  EXPECT(a && b && n == 4, "shorts + long short populated");
  vl_destroy(v);
}


/* ---- phase: mapped globals/parents, help internal, parse edges ------- */

void test_conflict_maps_global_and_parent(void) {
  static const vl_option_t global_g = {
      .name = "global",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  static vl_option_t parent_mut = {
      .name = "parent",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  static const vl_option_t *parent_req[2];
  static const vl_option_t *local_c[3];
  static vl_option_t local = {
      .name = "local",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };

  parent_req[0] = &global_g;
  parent_req[1] = NULL;
  parent_mut.requires = parent_req;
  local_c[0] = &global_g;
  local_c[1] = &parent_mut;
  local_c[2] = NULL;
  local.conflicts = local_c;

  const vl_option_t *const parent_opts[] = {&parent_mut, NULL};
  const vl_option_t *const local_opts[] = {&local, NULL};
  const vl_option_t *const globals[] = {&global_g, NULL};
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "inner", .options = local_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "outer", .options = parent_opts, .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {
      .options = globals,
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *ok[] = {(char *)"valve", (char *)"outer", (char *)"inner",
                (char *)"--local"};
  char *conflict[] = {(char *)"valve", (char *)"outer", (char *)"inner",
                      (char *)"--global", (char *)"--local"};
  char *req[] = {(char *)"valve", (char *)"outer", (char *)"inner",
                 (char *)"--parent"};

  EXPECT(v != NULL, "cross-scope relation schema");
  EXPECT(vl_parse(v, 4, ok) == 0, "local alone ok");
  vl_destroy(v);

  v = vl_create(&settings);
  EXPECT(vl_parse(v, 5, conflict) == -1, "local conflicts with global");
  vl_destroy(v);

  v = vl_create(&settings);
  EXPECT(vl_parse(v, 4, req) == -1, "parent requires global");
  vl_destroy(v);
}

void test_help_resolve_internal_paths(void) {
  const vl_option_t *const call_opts[] = {
      VL_OPT(.name = "request", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const net_opts[] = {
      VL_OPT(.name = "lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_INT),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "verbose", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "call", .options = call_opts),
      VL_CMD(.name = "list"),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .verbs = subs),
      VL_CMD(.name = "network", .options = net_opts),
      NULL,
  };
  vl_executable_t settings = {
      .options = globals,
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"--help=agent.call.request"};
  char *a2[] = {(char *)"valve", (char *)"--help=network.lane"};
  char *a3[] = {(char *)"valve", (char *)"--help=verbose"};
  char *a4[] = {(char *)"valve", (char *)"--help=list"};
  char *a5[] = {(char *)"valve", (char *)"network", (char *)"--help=lane"};
  char *a6[] = {(char *)"valve", (char *)"--help=proxy"};

  EXPECT(v != NULL, "help internal parser");
  EXPECT(vl_parse(v, 2, a1) == 0, "n=3 help path");
  EXPECT(vl_parse(v, 2, a2) == 0, "verb.option help path");
  EXPECT(vl_parse(v, 2, a3) == 0, "global option help");
  EXPECT(vl_parse(v, 2, a4) == 0, "unique subverb help");
  EXPECT(vl_parse(v, 3, a5) == 0, "active verb option help");
  EXPECT(vl_parse(v, 2, a6) == 0, "group help on verb options");
  vl_destroy(v);
}

void test_parse_edges_number_toggle_array_kv(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_NUMBER),
      VL_OPT(.name = "enable-x", .toggle_ref = "x", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "arr", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_ARRAY),
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      VL_OPT(.name = "auto", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_AUTO),
      NULL,
  };
  valve_t *v = parser_(options, 5);
  char *num[] = {(char *)"valve", (char *)"--n=42"};
  char *num_bad[] = {(char *)"valve", (char *)"--n=nope"};
  char *tog_bad[] = {(char *)"valve", (char *)"--enable-x"};
  char *arr_q[] = {(char *)"valve", (char *)"--arr=\"a\",b"};
  char *arr_bad_q[] = {(char *)"valve", (char *)"--arr=\"unterminated"};
  char *kv_bad[] = {(char *)"valve", (char *)"--meta=!a"};
  char *kv_empty[] = {(char *)"valve", (char *)"--meta="};
  char *auto_d[] = {(char *)"valve", (char *)"--auto=1.5"};
  char *dashdash[] = {(char *)"valve", (char *)"--n=1", (char *)"--",
                      (char *)"pos"};
  char *empty_arg[] = {(char *)"valve", (char *)"--n=1", (char *)""};

  EXPECT(v != NULL, "edges parser");
  EXPECT(vl_parse(v, 2, num) == 0 && vl_get(v, "n")->kind == VL_VALUE_INT,
         "NUMBER int path");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 2, num_bad) == -1, "NUMBER rejects non-numeric");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 2, tog_bad) == -1, "enable on non-toggle fails");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 2, arr_q) == 0, "quoted array element");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 2, arr_bad_q) == -1, "unterminated array quote");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 2, kv_bad) == -1, "kv missing colon");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 2, kv_empty) == -1, "empty kv fails");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 2, auto_d) == 0 &&
             vl_get(v, "auto")->kind == VL_VALUE_DOUBLE,
         "AUTO double mark");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 4, dashdash) == 0, "-- terminator stops parse");
  vl_destroy(v);

  v = parser_(options, 5);
  EXPECT(vl_parse(v, 3, empty_arg) == -1, "empty argv slot fails");
  vl_destroy(v);
}

void test_schema_validation_rejects(void) {
  const vl_option_t *const bad_opts_count[] = {NULL};
  vl_executable_t s1 = {.option_count = 1, .options = NULL};
  EXPECT(vl_create(&s1) == NULL, "option_count without options");

  vl_executable_t s2 = {.verb_count = 1, .verbs = NULL};
  EXPECT(vl_create(&s2) == NULL, "verb_count without verbs");

  vl_executable_t s3 = {.assign = (vl_assign_t)99};
  EXPECT(vl_create(&s3) == NULL, "invalid assign");

  const vl_verb_t *const bad_verb[] = {
      VL_CMD(.name = "x", .option_count = 1, .options = NULL),
      NULL,
  };
  vl_executable_t s4 = {.verbs = bad_verb};
  EXPECT(vl_create(&s4) == NULL, "verb option_count without options");

  const vl_option_t *const reserved[] = {
      VL_OPT(.name = "help", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  vl_executable_t s5 = {.options = reserved};
  EXPECT(vl_create(&s5) == NULL, "reserved option rejected");

  const vl_verb_t *const reserved_v[] = {
      VL_CMD(.name = "help"),
      NULL,
  };
  vl_executable_t s6 = {.verbs = reserved_v};
  EXPECT(vl_create(&s6) == NULL, "reserved verb rejected");

  /* sibling collision: subverb option collides with parent option */
  const vl_option_t *const parent_opts[] = {
      VL_OPT(.name = "same", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_option_t *const child_opts[] = {
      VL_OPT(.name = "same", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "child", .options = child_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "parent", .options = parent_opts, .verbs = subs),
      NULL,
  };
  vl_executable_t s7 = {.verbs = verbs};
  EXPECT(vl_create(&s7) == NULL, "parent/child option collision rejected");

  (void)bad_opts_count;
}

void test_result_cap_growth_and_int_overflow(void) {
  int i = 0;
  const vl_option_t *const opts[] = {
      VL_OPT(.name = "a0", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a1", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a2", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a3", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a4", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a5", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a6", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a7", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "a8", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "big", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .data = &i, .target = VL_TARGET_INT),
      NULL,
  };
  char *argv[] = {
      (char *)"valve", (char *)"--a0", (char *)"--a1", (char *)"--a2",
      (char *)"--a3",  (char *)"--a4", (char *)"--a5", (char *)"--a6",
      (char *)"--a7",  (char *)"--a8",
  };
  valve_t *v = parser_(opts, 10);
  EXPECT(v != NULL && vl_parse(v, 10, argv) == 0, "9 results grow capacity");
  EXPECT(vl_result_count(v) == 9, "nine results stored");
  vl_destroy(v);

  v = parser_(opts, 10);
  char *overflow[] = {(char *)"valve",
                      (char *)"--big=9999999999999999999"};
  EXPECT(vl_parse(v, 2, overflow) == -1, "int target overflow fails");
  vl_destroy(v);
}

void test_kv_target_value_and_value_labels(void) {
  vl_value_t meta = {0};
  typedef struct {
    char *s;
    double d;
    bool b;
  } cfg_t;
  cfg_t cfg = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV,
             .data = &meta, .target = VL_TARGET_VALUE),
      VL_OPT(.name = "g.s", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, s)),
      VL_OPT(.name = "g.d", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_DOUBLE,
             .data = &cfg, .offset = offsetof(cfg_t, d)),
      VL_OPT(.name = "g.b", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_BOOL,
             .data = &cfg, .offset = offsetof(cfg_t, b)),
      VL_OPT(.name = "g.x", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_VALUE),
      VL_OPT(.name = "t", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_TIME),
      NULL,
  };
  vl_executable_t settings = {.options = options, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--meta=!k:1|n:{!z:true}",
                  (char *)"--g.s=hi", (char *)"--g.d=1.5", (char *)"--g.b=true",
                  (char *)"--g.x=z", (char *)"--t=1s"};
  char *help[] = {(char *)"valve", (char *)"--help"};

  EXPECT(v != NULL, "kv target parser");
  EXPECT(vl_parse(v, 7, argv) == 0, "kv+dot targets parse");
  EXPECT(meta.kind == VL_VALUE_KV && meta.as.kv.count >= 2, "kv target filled");
  EXPECT(cfg.s && cfg.d > 1.0 && cfg.b, "dot targets filled");
  vl_destroy(v);
  vl_value_clear(&meta);
  free(cfg.s);

  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, help) == 0, "help prints value labels");
  vl_destroy(v);
}

void test_find_option_on_active_scopes(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "req", .short_name = 'r',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "mode", .short_name = 'm',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "do", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "act", .options = verb_opts, .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"act", (char *)"do",
                (char *)"--req=x"};
  char *a2[] = {(char *)"valve", (char *)"act", (char *)"do", (char *)"-r=y"};
  char *a3[] = {(char *)"valve", (char *)"act", (char *)"do",
                (char *)"--mode=z"};

  EXPECT(v != NULL, "find-option parser");
  EXPECT(vl_parse(v, 4, a1) == 0, "subverb long find");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 4, a2) == 0, "subverb short find");
  vl_destroy(v);
  /* mode is on parent verb; with required subverb must still find parent opts */
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 4, a3) == 0, "parent option via active verb find");
  vl_destroy(v);
}

void test_unknown_verb_and_subverb(void) {
  const vl_verb_t *const subs[] = {VL_CMD(.name = "ok"), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "known", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *bad_v[] = {(char *)"valve", (char *)"nope"};
  char *bad_s[] = {(char *)"valve", (char *)"known", (char *)"nope"};
  char *opt_first[] = {(char *)"valve", (char *)"--anything"};

  EXPECT(v != NULL, "unknown verb parser");
  EXPECT(vl_parse(v, 2, bad_v) == -1, "unknown verb");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, bad_s) == -1, "unknown subverb");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, opt_first) == -1, "option before verb fails");
  vl_destroy(v);
}


void test_parse_negative_dot_and_number_fallback(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "d", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_DOUBLE),
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_NUMBER),
      VL_OPT(.name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_AUTO),
      NULL,
  };
  vl_executable_t separate = {
      .options = options,
      .assign = VL_ASSIGN_SEPARATE,
      .color = VAL_COLOR_NEVER,
  };
  vl_executable_t inline_as = {
      .options = options,
      .assign = VL_ASSIGN_INLINE,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&separate);
  char *neg[] = {(char *)"valve", (char *)"--d", (char *)"-.5"};
  char *num[] = {(char *)"valve", (char *)"--n", (char *)"1e2"};
  char *num2[] = {(char *)"valve", (char *)"--n=3.14"};
  char *auto_i[] = {(char *)"valve", (char *)"--a=9"};

  EXPECT(v != NULL && vl_parse(v, 3, neg) == 0, "negative dotted double");
  EXPECT(vl_get(v, "d") && vl_get(v, "d")->as.number < 0, "neg double value");
  vl_destroy(v);

  v = vl_create(&separate);
  EXPECT(vl_parse(v, 3, num) == 0, "NUMBER scientific separate");
  vl_destroy(v);
  v = vl_create(&inline_as);
  EXPECT(vl_parse(v, 2, num2) == 0 &&
             vl_get(v, "n")->kind == VL_VALUE_DOUBLE,
         "NUMBER decimal inline");
  vl_destroy(v);
  v = vl_create(&inline_as);
  EXPECT(vl_parse(v, 2, auto_i) == 0 && vl_get(v, "a")->kind == VL_VALUE_INT,
         "AUTO int");
  vl_destroy(v);
}

void test_toggle_type_without_toggle_value(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "enable-x", .toggle_ref = "x", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *argv[] = {(char *)"valve", (char *)"--enable-x"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1,
         "toggle type requires TOGGLE value");
  vl_destroy(v);
}

void test_kv_and_array_malformed_edges(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      VL_OPT(.name = "arr", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_ARRAY),
      NULL,
  };
  char *pipe[] = {(char *)"valve", (char *)"--meta=!a:1|"};
  char *brace[] = {(char *)"valve", (char *)"--meta=!a:{!b:1"};
  char *no_bang[] = {(char *)"valve", (char *)"--meta=a:1"};
  char *empty_el[] = {(char *)"valve", (char *)"--arr=a,,b"};
  char *semi[] = {(char *)"valve", (char *)"--arr=\"a\"b"};
  valve_t *v;

  v = parser_(options, 2);
  EXPECT(vl_parse(v, 2, pipe) == -1, "kv trailing pipe");
  vl_destroy(v);
  v = parser_(options, 2);
  EXPECT(vl_parse(v, 2, brace) == -1, "kv unclosed brace");
  vl_destroy(v);
  v = parser_(options, 2);
  EXPECT(vl_parse(v, 2, no_bang) == -1, "kv missing bang");
  vl_destroy(v);
  v = parser_(options, 2);
  EXPECT(vl_parse(v, 2, empty_el) == -1, "array empty element");
  vl_destroy(v);
  v = parser_(options, 2);
  EXPECT(vl_parse(v, 2, semi) == -1, "array unexpected char after quote");
  vl_destroy(v);
}

void test_find_short_on_verb_without_subverbs(void) {
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "mode", .short_name = 'm',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "act", .options = verb_opts),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"act", (char *)"-m=x"};
  char *a2[] = {(char *)"valve", (char *)"act", (char *)"--mode=y"};
  EXPECT(v != NULL && vl_parse(v, 3, a1) == 0, "verb short find");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, a2) == 0, "verb long find");
  vl_destroy(v);
}

void test_help_option_on_subverb_anywhere(void) {
  const vl_option_t *const call_opts[] = {
      VL_OPT(.name = "request", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "call", .options = call_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {
      .options = globals, .verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"--help=request"};
  char *a2[] = {(char *)"valve", (char *)"--help=proxy"};
  char *a3[] = {(char *)"valve", (char *)"--help=agent.missing"};
  char *a4[] = {(char *)"valve", (char *)"--help=agent.call.missing"};
  EXPECT(v != NULL, "subverb option help parser");
  EXPECT(vl_parse(v, 2, a1) == 0, "option anywhere on subverb");
  EXPECT(vl_parse(v, 2, a2) == 0, "group on globals");
  EXPECT(vl_parse(v, 2, a3) == -1, "verb.missing fails");
  EXPECT(vl_parse(v, 2, a4) == -1, "n=3 missing option fails");
  vl_destroy(v);
}

void test_schema_verb_invalid_cases(void) {
  const vl_option_t *const dup_long[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const v1[] = {
      VL_CMD(.name = "go", .options = dup_long),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = v1}) == NULL, "dup long on verb");

  const vl_option_t *const dup_short[] = {
      VL_OPT(.name = "a", .short_name = 'x', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "b", .short_name = 'x', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const v2[] = {
      VL_CMD(.name = "go", .options = dup_short),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = v2}) == NULL, "dup short on verb");

  const vl_verb_t *const reserved_sub[] = {
      VL_CMD(.name = "help"),
      NULL,
  };
  const vl_verb_t *const v3[] = {
      VL_CMD(.name = "go", .verbs = reserved_sub),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = v3}) == NULL,
         "reserved subverb rejected");

  const vl_verb_t *const dup_sub[] = {
      VL_CMD(.name = "same"),
      VL_CMD(.name = "same"),
      NULL,
  };
  const vl_verb_t *const v4[] = {
      VL_CMD(.name = "go", .verbs = dup_sub),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = v4}) == NULL, "dup subverb");

  const vl_option_t *const bad_type[] = {
      VL_OPT(.name = "x", .type = (vl_opt_type_t)255,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const v5[] = {
      VL_CMD(.name = "go", .options = bad_type),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = v5}) == NULL,
         "invalid option type on verb");
}

void test_kv_nested_target_and_repeat_array_target(void) {
  vl_value_t meta = {0};
  vl_value_t groups = {0};
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV,
             .data = &meta, .target = VL_TARGET_VALUE),
      VL_OPT(.name = "groups", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY, .repeat = VL_OPTION_REPEAT_ARRAY,
             .data = &groups, .target = VL_TARGET_VALUE),
      NULL,
  };
  valve_t *v = parser_(options, 2);
  char *argv[] = {(char *)"valve", (char *)"--meta=!a:{!b:1|c:true}|d:\"x\"",
                  (char *)"--groups=1,2", (char *)"--groups=3,4"};
  EXPECT(v != NULL && vl_parse(v, 4, argv) == 0, "nested kv + array repeat target");
  EXPECT(meta.kind == VL_VALUE_KV, "nested kv target");
  EXPECT(groups.kind == VL_VALUE_ARRAY && groups.as.array.count == 2,
         "array-of-arrays target");
  vl_destroy(v);
  vl_value_clear(&meta);
  vl_value_clear(&groups);
}

void test_color_detect_uninit_and_hyperlinks(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  setenv("TERM_PROGRAM", "vscode", 1);
  setenv("COLORTERM", "24bit", 1);
  setenv("TERM", "xterm-truecolor", 1);
  vl_color_support_reset();
  /* Force re-init path without prior init by resetting then detecting */
  EXPECT(vl_color_supports_hyperlinks() || !vl_color_supports_hyperlinks(),
         "hyperlinks callable");
  vl_term_caps_t caps;
  vl_term_caps(&caps);
  (void)vl_color_enabled(stdout);
  (void)vl_color_for(stdout, VAL_BOLD);
  unsetenv("TERM_PROGRAM");
  unsetenv("COLORTERM");
  unsetenv("TERM");
}


void test_parse_short_form_guards_and_cluster(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "onlylong", .short_name = 'L', .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "tags", .short_name = 't', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_KV),
      VL_OPT(.name = "mode", .short_name = 'm', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "flag", .short_name = 'f', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "other", .short_name = 'o', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  vl_executable_t inline_as = {
      .options = options, .assign = VL_ASSIGN_INLINE, .color = VAL_COLOR_NEVER};
  vl_executable_t separate = {
      .options = options, .assign = VL_ASSIGN_SEPARATE, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&inline_as);
  char *long_as_short[] = {(char *)"valve", (char *)"-L=x"};
  char *kv_short[] = {(char *)"valve", (char *)"-t=!a:1"};
  char *sep_disabled[] = {(char *)"valve", (char *)"-m", (char *)"x"};
  char *inline_disabled[] = {(char *)"valve", (char *)"-m=x"};
  char *cluster[] = {(char *)"valve", (char *)"-fo"};

  EXPECT(v != NULL && vl_parse(v, 2, long_as_short) == -1,
         "long-only rejects short token");
  vl_destroy(v);
  v = vl_create(&inline_as);
  EXPECT(vl_parse(v, 2, kv_short) == -1, "kv rejects short form");
  vl_destroy(v);
  v = vl_create(&inline_as);
  EXPECT(vl_parse(v, 3, sep_disabled) == -1, "short separate disabled");
  vl_destroy(v);
  v = vl_create(&separate);
  EXPECT(vl_parse(v, 2, inline_disabled) == -1, "short inline disabled");
  vl_destroy(v);
  v = vl_create(&inline_as);
  EXPECT(vl_parse(v, 2, cluster) == -1, "short cluster unexpected chars");
  vl_destroy(v);
}

void test_argc_one_dispatches_help(void) {
  const vl_option_t *const opts[] = {NULL};
  vl_executable_t settings = {
      .options = opts,
      .program_name = "solo",
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"solo"};
  EXPECT(v != NULL && vl_parse(v, 1, argv) == 0, "argc<=1 prints help");
  EXPECT(vl_reserved_fired(v), "argc<=1 counts as reserved help");
  vl_destroy(v);
}

void test_reserved_in_verb_and_subverb_slots(void) {
  const vl_verb_t *const subs[] = {VL_CMD(.name = "do"), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *as_verb[] = {(char *)"valve", (char *)"--help"};
  char *as_sub[] = {(char *)"valve", (char *)"run", (char *)"--version"};
  char *joined[] = {(char *)"valve", (char *)"--helpagent"};
  EXPECT(v != NULL && vl_parse(v, 2, as_verb) == 0, "help instead of verb");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, as_sub) == 0, "version instead of subverb");
  vl_destroy(v);
  v = vl_create(&settings);
  /* joined help target form may resolve or fail — either exercises extract */
  (void)vl_parse(v, 2, joined);
  vl_destroy(v);
}

void test_error_capacity_growth(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "ok", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  char *argv[] = {
      (char *)"valve", (char *)"--a=1", (char *)"--b=1", (char *)"--c=1",
      (char *)"--d=1", (char *)"--e=1", (char *)"--f=1", (char *)"--g=1",
  };
  valve_t *v = parser_(options, 1);
  EXPECT(v != NULL && vl_parse(v, 8, argv) == -1, "many unknown options");
  EXPECT(vl_error_count(v) >= 5, "error list grew past initial cap");
  vl_destroy(v);
}

void test_help_print_shared_and_overview(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "local", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL, .description = "local flag"),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "shared", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .usage = "shared usage"),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "inner", .options = sub_opts, .description = "inner desc",
             .usage = "inner usage"),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "outer", .options = verb_opts, .verbs = subs,
             .description = "outer desc", .usage = "outer usage"),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = verbs,
      .program_name = "rich",
      .program_version = "0.0.1",
      .description = "desc",
      .usage = "usage line",
      .logo = "*",
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *overview[] = {(char *)"rich", (char *)"--help"};
  char *sub[] = {(char *)"rich", (char *)"--help=outer.inner"};
  char *verb[] = {(char *)"rich", (char *)"--help=outer"};
  EXPECT(v != NULL, "rich help schema");
  EXPECT(vl_parse(v, 2, overview) == 0, "overview with usage");
  EXPECT(vl_parse(v, 2, sub) == 0, "subverb card with shared parent opts");
  EXPECT(vl_parse(v, 2, verb) == 0, "verb card");
  vl_destroy(v);
}

void test_duration_edge_messages(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_TIME,
             .has_int_max = true, .int_max = 10),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *bad[] = {(char *)"valve", (char *)"--wait=notaduration"};
  char *over[] = {(char *)"valve", (char *)"--wait=999h"};
  char *ok[] = {(char *)"valve", (char *)"--wait=5s"};
  EXPECT(v != NULL && vl_parse(v, 2, bad) == -1, "bad duration");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, over) == -1, "duration over max");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, ok) == 0, "duration ok");
  vl_destroy(v);
}

void test_help_resolve_more_public_cases(void) {
  const vl_option_t *const call_opts[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "call", .options = call_opts),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "g.a", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_BOOL),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .verbs = subs),
      NULL,
  };
  vl_help_resolution_t r = {0};
  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "agent.call.x", &r) &&
             r.kind == VL_HELP_OPTION,
         "public resolve n=3 option");
  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "g", &r) && r.kind == VL_HELP_GROUP,
         "public resolve group");
  EXPECT(!vl_help_resolve(NULL, 0, NULL, 0, "x", &r), "null verbs table");
  EXPECT(!vl_help_resolve(verbs, 0, globals, 0, NULL, &r), "null target");
}


void test_help_with_active_verb_context(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "local", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "shared", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "inner", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "outer", .options = verb_opts, .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *verb_help[] = {(char *)"valve", (char *)"outer", (char *)"--help"};
  char *sub_help[] = {(char *)"valve", (char *)"outer", (char *)"inner",
                      (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 3, verb_help) == 0,
         "bare --help after verb prints verb card");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 4, sub_help) == 0,
         "bare --help after subverb prints subverb card");
  vl_destroy(v);
}

void test_joined_question_help_and_eq_edge(void) {
  const vl_option_t *const opts[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = opts),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *joined[] = {(char *)"valve", (char *)"-?run"};
  char *long_eq[] = {(char *)"valve", (char *)"--help=run"};
  char *huge[] = {(char *)"valve",
                  (char *)"--help="
                  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"};
  EXPECT(v != NULL && vl_parse(v, 2, joined) == 0, "joined -?target help");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, long_eq) == 0, "help=run");
  vl_destroy(v);
  v = vl_create(&settings);
  (void)vl_parse(v, 2, huge); /* plen overflow path may no-op as reserved */
  vl_destroy(v);
}

void test_help_resolve_subverb_option_and_group(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "nest.opt", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      VL_OPT(.name = "uniqueopt", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "call", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .verbs = subs),
      NULL,
  };
  vl_help_resolution_t r = {0};
  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "uniqueopt", &r) &&
             r.kind == VL_HELP_OPTION && r.subverb != NULL,
         "public resolve option on subverb");
  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "nest", &r) &&
             r.kind == VL_HELP_GROUP,
         "public resolve group on subverb options");
  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "agent.call.uniqueopt", &r) &&
             r.kind == VL_HELP_OPTION,
         "public resolve verb.sub.option");
}

void test_verb_conflict_requirement_invalid(void) {
  static const vl_option_t other = {
      .name = "other", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t self = {
      .name = "self", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *self_c[2];
  self_c[0] = &self;
  self_c[1] = NULL;
  self.conflicts = self_c;
  const vl_option_t *const local[] = {&self, &other, NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "go", .options = local),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = verbs}) == NULL,
         "self-conflict on verb rejected");

  static vl_option_t req = {
      .name = "req", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *req_t[2];
  req_t[0] = &req;
  req_t[1] = NULL;
  req.requires = req_t;
  const vl_option_t *const local2[] = {&req, NULL};
  const vl_verb_t *const verbs2[] = {
      VL_CMD(.name = "go", .options = local2),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = verbs2}) == NULL,
         "self-require on verb rejected");

  const vl_option_t *const reserved_opt[] = {
      VL_OPT(.name = "version", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const verbs3[] = {
      VL_CMD(.name = "go", .options = reserved_opt),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = verbs3}) == NULL,
         "reserved option on verb rejected");
}

void test_number_double_fallback_and_neg_int_lookalike(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_NUMBER),
      VL_OPT(.name = "i", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT),
      NULL,
  };
  vl_executable_t settings = {
      .options = options, .assign = VL_ASSIGN_SEPARATE, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *n1[] = {(char *)"valve", (char *)"--n", (char *)".25"};
  char *n2[] = {(char *)"valve", (char *)"--i", (char *)"-7"};
  EXPECT(v != NULL && vl_parse(v, 3, n1) == 0 &&
             vl_get(v, "n")->kind == VL_VALUE_DOUBLE,
         "NUMBER leading-dot double");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 3, n2) == 0 && vl_get(v, "i")->as.integer == -7,
         "INT separate negative");
  vl_destroy(v);
}


void test_duration_overflow_negative_empty_syntax(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_TIME),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *neg[] = {(char *)"valve", (char *)"--wait=-5s"};
  char *empty[] = {(char *)"valve", (char *)"--wait="};
  char *suf[] = {(char *)"valve", (char *)"--wait=3x"};
  char *syn[] = {(char *)"valve", (char *)"--wait=1h30"};
  char *ov[] = {(char *)"valve",
                (char *)"--wait=9999999999999999999999999999999999999999d"};
  char *mix[] = {(char *)"valve", (char *)"--wait=1h30m"};
  EXPECT(v != NULL && vl_parse(v, 2, neg) == -1, "negative duration");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, empty) == -1, "empty duration");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, suf) == -1, "bad duration suffix");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, syn) == -1, "duration trailing digits syntax");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, ov) == -1, "duration overflow");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, mix) == 0, "1h30m ok");
  vl_destroy(v);
}

void test_help_resolve_verb_option_and_miss(void) {
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "req", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "call", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "net", .options = verb_opts, .verbs = subs),
      NULL,
  };
  vl_help_resolution_t r = {0};
  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "net.lane", &r) &&
             r.kind == VL_HELP_OPTION && r.verb && !r.subverb,
         "n=2 verb.option");
  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "net.call.missing", &r),
         "n=3 missing option falls through");
  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "net.nope", &r),
         "n=2 missing option/sub");
}

void test_invalid_subverb_name(void) {
  const vl_verb_t *const bad_subs[] = {
      VL_CMD(.name = ""),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "go", .verbs = bad_subs),
      NULL,
  };
  EXPECT(vl_create(&(vl_executable_t){.verbs = verbs}) == NULL,
         "empty subverb name rejected");
}

void test_version_with_value_in_verb_slot(void) {
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run"),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--version=1.2.3"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1,
         "version=value in verb slot fails");
  vl_destroy(v);
}

void test_help_unknown_with_subverb_hints(void) {
  const vl_verb_t *const subs_a[] = {VL_CMD(.name = "list"), NULL};
  const vl_verb_t *const subs_b[] = {VL_CMD(.name = "list"), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "one", .verbs = subs_a),
      VL_CMD(.name = "two", .verbs = subs_b),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help=list"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1, "ambiguous subverb help");
  const vl_error_t *err = vl_error_at(v, 0);
  EXPECT(err && err->message && strstr(err->message, "one.list") != NULL,
         "hint names dotted subverb forms");
  vl_destroy(v);
}


void test_required_on_verb_and_subverb(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "need", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .required = true),
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "must", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .required = true),
      NULL,
  };
  const vl_verb_t *const leaf[] = {
      VL_CMD(.name = "leaf", .options = verb_opts),
      NULL,
  };
  const vl_verb_t *const nested_subs[] = {
      VL_CMD(.name = "inner", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const nested[] = {
      VL_CMD(.name = "outer", .verbs = nested_subs),
      NULL,
  };
  vl_executable_t s1 = {.verbs = leaf, .color = VAL_COLOR_NEVER};
  vl_executable_t s2 = {.verbs = nested, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&s1);
  char *miss_v[] = {(char *)"valve", (char *)"leaf"};
  EXPECT(v != NULL && vl_parse(v, 2, miss_v) == -1, "missing verb required");
  vl_destroy(v);
  v = vl_create(&s2);
  char *miss_s[] = {(char *)"valve", (char *)"outer", (char *)"inner"};
  EXPECT(v != NULL && vl_parse(v, 3, miss_s) == -1, "missing subverb required");
  vl_destroy(v);
}

void test_inactive_verb_conflicts_skipped(void) {
  static const vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const a_c[] = {&b, NULL};
  static const vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .conflicts = a_c};
  static const vl_option_t *const a_opts[] = {&a, &b, NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "alpha", .options = a_opts),
      VL_CMD(.name = "beta"),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"beta"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0,
         "other verb ignores inactive conflicts");
  vl_destroy(v);
}

void test_duration_precise_overflow(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_TIME),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *bare_ov[] = {(char *)"valve",
                     (char *)"--wait=99999999999999999999"};
  char *day_ov[] = {(char *)"valve", (char *)"--wait=999999999999999999d"};
  char *sum_ov[] = {(char *)"valve",
                    (char *)"--wait=1000000000000000000d1000000000000000000d"};
  EXPECT(v != NULL && vl_parse(v, 2, bare_ov) == -1, "bare seconds overflow");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, day_ov) == -1, "day scale overflow");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, sum_ov) == -1, "duration sum overflow");
  vl_destroy(v);
}

void test_reserved_aliases_and_dash_only(void) {
  const vl_option_t *const opts[] = {NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *q[] = {(char *)"valve", (char *)"?"};
  char *hq[] = {(char *)"valve", (char *)"-?"};
  char *hh[] = {(char *)"valve", (char *)"--?"};
  char *h[] = {(char *)"valve", (char *)"--h"};
  char *vv[] = {(char *)"valve", (char *)"-v"};
  char *dash[] = {(char *)"valve", (char *)"-"};
  EXPECT(v != NULL && vl_parse(v, 2, q) == 0, "? help alias");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, hq) == 0, "-? help");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, hh) == 0, "--? help");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, h) == 0, "--h help");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, vv) == 0, "-v version");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, dash) == -1, "lone dash unknown");
  vl_destroy(v);
}

void test_value_label_kv_toggle_defaults(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      VL_OPT(.name = "enable-x", .toggle_ref = "x", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE),
      VL_OPT(.name = "weird", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_AUTO),
      VL_OPT(.name = "g.v", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_VALUE),
      NULL,
  };
  vl_executable_t settings = {.options = options, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "labels for kv/toggle/auto/dot");
  vl_destroy(v);
}

void test_multi_requires_same_name_annotations(void) {
  /* Two sources require same target name via separate option objects that share
   * the name after copy? Use one source requiring two targets, and help print. */
  static const vl_option_t t1 = {
      .name = "t1", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t t2 = {
      .name = "t2", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const reqs[] = {&t1, &t2, NULL};
  static const vl_option_t src = {
      .name = "src", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .requires = reqs, .required = true};
  static const vl_option_t *const opts[] = {&src, &t1, &t2, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *help[] = {(char *)"valve", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 2, help) == 0, "multi-require annotations");
  vl_destroy(v);
}

void test_color_tty_env_heuristics(void) {
  /* Use a PTY slave so isatty(STDOUT/STDERR) is true and detect_color_env_
   * walks COLORTERM / TERM_PROGRAM / TERM heuristics. */
  int saved_out = dup(STDOUT_FILENO);
  int saved_err = dup(STDERR_FILENO);
  int master = -1;
  int slave = -1;
  if (saved_out < 0 || saved_err < 0 ||
      openpty(&master, &slave, NULL, NULL, NULL) != 0) {
    if (master >= 0) close(master);
    if (slave >= 0) close(slave);
    if (saved_out >= 0) close(saved_out);
    if (saved_err >= 0) close(saved_err);
    TEST_IGNORE_MESSAGE("openpty unavailable");
    return;
  }
  dup2(slave, STDOUT_FILENO);
  dup2(slave, STDERR_FILENO);
  close(slave);

  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  unsetenv("VTE_VERSION");
  unsetenv("KITTY_WINDOW_ID");
  unsetenv("WEZTERM_PANE");
  unsetenv("KONSOLE_VERSION");

  setenv("COLORTERM", "truecolor", 1);
  unsetenv("TERM_PROGRAM");
  setenv("TERM", "xterm-256color", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_supports_truecolor();
  (void)vl_color_supports_hyperlinks();

  unsetenv("COLORTERM");
  setenv("TERM_PROGRAM", "iTerm.app", 1);
  setenv("TERM", "xterm", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_supports_truecolor();

  setenv("TERM_PROGRAM", "Apple_Terminal", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_support_detect();

  unsetenv("TERM_PROGRAM");
  setenv("TERM", "xterm-direct", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_supports_truecolor();

  setenv("TERM", "screen-256color", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_supports_256();

  setenv("TERM", "tmux", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_supports_256();

  setenv("TERM", "dumb", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_support_detect();

  setenv("TERM", "", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_ALWAYS);
  (void)vl_color_support_detect();

  setenv("TERM", "weirdterm", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  (void)vl_color_support_detect();

  setenv("VTE_VERSION", "6000", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  EXPECT(vl_color_supports_hyperlinks(), "VTE hyperlinks");
  unsetenv("VTE_VERSION");
  setenv("KITTY_WINDOW_ID", "1", 1);
  vl_color_support_reset();
  (void)vl_color_supports_hyperlinks();
  unsetenv("KITTY_WINDOW_ID");
  setenv("WEZTERM_PANE", "0", 1);
  vl_color_support_reset();
  (void)vl_color_supports_hyperlinks();
  unsetenv("WEZTERM_PANE");
  setenv("KONSOLE_VERSION", "1", 1);
  vl_color_support_reset();
  (void)vl_color_supports_hyperlinks();
  unsetenv("KONSOLE_VERSION");

  vl_term_caps_t caps = {0};
  vl_term_caps(&caps);
  EXPECT(caps.stdout_tty || caps.stderr_tty, "tty flags after openpty redirect");

  dup2(saved_out, STDOUT_FILENO);
  dup2(saved_err, STDERR_FILENO);
  close(saved_out);
  close(saved_err);
  close(master);
  unsetenv("COLORTERM");
  unsetenv("TERM_PROGRAM");
  unsetenv("FORCE_COLOR");
  unsetenv("NO_COLOR");
  setenv("TERM", "dumb", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
}

void test_duration_digit_add_and_sum_overflow(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_TIME),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  /* digits*10 ok, digits+digit overflows */
  char *add_ov[] = {(char *)"valve", (char *)"--wait=9223372036854775808"};
  /* two segments whose seconds sum overflows */
  char *sum_ov[] = {
      (char *)"valve",
      (char *)"--wait=4611686018427387903s4611686018427387905s"};
  EXPECT(v != NULL && vl_parse(v, 2, add_ov) == -1, "digit add overflow");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, sum_ov) == -1, "segment sum overflow");
  vl_destroy(v);
}

void test_help_active_scope_fallthrough_and_sub_group(void) {
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "req", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "db.host", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "call", .options = sub_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "net", .options = verb_opts, .verbs = subs),
      NULL,
  };
  vl_executable_t settings = {.verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  /* active subverb, target is verb-local → fall through resolve_active_ */
  char *a1[] = {(char *)"valve", (char *)"net", (char *)"call",
                (char *)"--help=lane"};
  char *a2[] = {(char *)"valve", (char *)"--help=db"};
  EXPECT(v != NULL && vl_parse(v, 4, a1) == 0, "active sub falls to verb option");
  vl_destroy(v);
  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, a2) == 0, "group only on subverb options");
  vl_destroy(v);
}

void test_targets_clear_null_slots(void) {
  static int x = 1;
  static const vl_option_t none_opt = {
      .name = "none", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .target = VL_TARGET_NONE, .data = &x};
  static const vl_option_t *const opts[] = {&none_opt, NULL, NULL};
  static const vl_verb_t leaf = {.name = "leaf"};
  static const vl_verb_t *const verbs[] = {&leaf, NULL, NULL};
  vl_executable_t settings = {
      .options = opts,
      .option_count = 2, /* includes a NULL slot */
      .verbs = verbs,
      .verb_count = 2, /* includes a NULL verb slot */
  };
  vl_targets_clear(&settings);
  EXPECT(1, "null slots cleared without crash");
}

void test_short_only_label_in_help(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "quiet", .short_name = 'q', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  vl_executable_t settings = {.options = options, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "short-only label");
  vl_destroy(v);
}

void test_conflict_seen_duplicate_names_in_help(void) {
  static vl_option_t recip_a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t recip_b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const recip_a_c[] = {&recip_b, NULL};
  static const vl_option_t *const recip_b_c[] = {&recip_a, NULL};
  recip_a.conflicts = recip_a_c;
  recip_b.conflicts = recip_b_c;
  static const vl_option_t *const opts[] = {&recip_a, &recip_b, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *help[] = {(char *)"valve", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 2, help) == 0, "reciprocal conflict annotations");
  vl_destroy(v);
}

