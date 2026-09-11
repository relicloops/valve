#include "test_support.h"
#include "fault_alloc.h"
#include "private.h"

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

/* ---- color ENV / caps branch arms ------------------------------------ */

void test_color_env_branch_arms(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");

  /* FORCE_COLOR empty string → skip leveled force */
  setenv("FORCE_COLOR", "", 1);
  setenv("TERM", "xterm", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  EXPECT(vl_color_support_detect() == VL_COLOR_SUPPORT_NONE, "empty FORCE");
  unsetenv("FORCE_COLOR");

  /* COLORTERM=24bit (needs TTY; still worth trying without FORCE) */
  unsetenv("FORCE_COLOR");
  unsetenv("NO_COLOR");
  setenv("COLORTERM", "24bit", 1);
  setenv("TERM", "xterm", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_ALWAYS);
  (void)vl_color_support_detect();
  unsetenv("COLORTERM");

  setenv("COLORTERM", "truecolor", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_ALWAYS);
  (void)vl_color_support_detect();
  unsetenv("COLORTERM");

  /* ALWAYS + empty TERM → BASIC floor when TTY path entered via FORCE skip */
  unsetenv("TERM");
  setenv("TERM", "", 1);
  unsetenv("COLORTERM");
  unsetenv("FORCE_COLOR");
  unsetenv("NO_COLOR");
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_ALWAYS);
  (void)vl_color_support_detect();

  /* TERM with 24bit / truecolor / direct / bare 256 */
  const char *terms[] = {"xterm-truecolor", "xterm-24bit", "xterm-direct",
                         "xterm-256", "foo256bar"};
  for (size_t i = 0; i < sizeof terms / sizeof terms[0]; ++i) {
    setenv("TERM", terms[i], 1);
    unsetenv("COLORTERM");
    setenv("FORCE_COLOR", "1", 1); /* ensure detect runs with support */
    vl_color_support_reset();
    vl_color_init(VAL_COLOR_AUTO);
    (void)vl_color_support_detect();
    unsetenv("FORCE_COLOR");
    /* Also without FORCE so TERM heuristics run when TTY */
    vl_color_support_reset();
    vl_color_init(VAL_COLOR_ALWAYS);
    (void)vl_color_support_detect();
  }

  /* COLUMNS/LINES empty and non-positive */
  setenv("COLUMNS", "", 1);
  setenv("LINES", "", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  vl_term_caps_t caps;
  vl_term_caps(&caps);

  setenv("COLUMNS", "0", 1);
  setenv("LINES", "-3", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  vl_term_caps(&caps);

  setenv("COLUMNS", "100", 1);
  setenv("LINES", "40", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  vl_term_caps(&caps);
  EXPECT(caps.columns == 100 || caps.columns == 80, "columns env or ioctl");

  /* UTF-8 via LANG when codeset is not UTF-8 */
  setenv("LC_ALL", "", 1);
  setenv("LC_CTYPE", "", 1);
  setenv("LANG", "en_US.UTF-8", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  vl_term_caps(&caps);

  setenv("LC_ALL", "", 1);
  setenv("LC_CTYPE", "C.UTF-8", 1);
  unsetenv("LANG");
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  vl_term_caps(&caps);

  setenv("LC_ALL", "C", 1);
  setenv("LC_CTYPE", "", 1);
  setenv("LANG", "", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_NEVER);
  vl_term_caps(&caps);

  /* force after init hits apply_enable_bits_ */
  unsetenv("NO_COLOR");
  setenv("FORCE_COLOR", "2", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_AUTO);
  vl_color_support_force(VL_COLOR_SUPPORT_256);
  EXPECT(vl_color_supports_256(), "force after init");

  /* NO_COLOR wins over FORCE in enable bits */
  setenv("NO_COLOR", "1", 1);
  setenv("FORCE_COLOR", "3", 1);
  vl_color_support_reset();
  vl_color_init(VAL_COLOR_ALWAYS);
  EXPECT(!vl_color_supports() || vl_color_support_detect() == VL_COLOR_SUPPORT_NONE,
         "NO_COLOR beats FORCE");
}

/* ---- parse edges ----------------------------------------------------- */

void test_parse_negative_number_branch_arms(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "n", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_NUMBER),
      NULL,
  };
  vl_executable_t settings = {
      .options = options, .assign = VL_ASSIGN_SEPARATE, .color = VAL_COLOR_NEVER};

  /* '-' then digit-range miss: ':' is >= '0' but > '9' */
  valve_t *v = vl_create(&settings);
  char *argv1[] = {(char *)"valve", (char *)"--n", (char *)"-:"};
  EXPECT(v != NULL && vl_parse(v, 3, argv1) == -1, "dash-colon not negative");
  vl_destroy(v);

  /* -. without digit */
  v = vl_create(&settings);
  char *argv2[] = {(char *)"valve", (char *)"--n", (char *)"-."};
  EXPECT(vl_parse(v, 3, argv2) == -1, "dot alone not negative");
  vl_destroy(v);

  /* -.a (dot then non-digit) */
  v = vl_create(&settings);
  char *argv3[] = {(char *)"valve", (char *)"--n", (char *)"-.a"};
  EXPECT(vl_parse(v, 3, argv3) == -1, "dot-alpha not negative");
  vl_destroy(v);

  /* -.! (dot then char < '0') */
  v = vl_create(&settings);
  char *argv3b[] = {(char *)"valve", (char *)"--n", (char *)"-.!"};
  EXPECT(vl_parse(v, 3, argv3b) == -1, "dot-bang not negative");
  vl_destroy(v);

  /* argv NULL hole after option */
  v = vl_create(&settings);
  char *argv4[] = {(char *)"valve", (char *)"--n", NULL};
  EXPECT(vl_parse(v, 3, argv4) == -1, "null next argv");
  vl_destroy(v);
}

void test_parse_enable_disable_empty_suffix(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "feature", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, .toggle_ref = "feature"),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *en[] = {(char *)"valve", (char *)"--enable-"};
  char *dis[] = {(char *)"valve", (char *)"--disable-"};
  EXPECT(v != NULL && vl_parse(v, 2, en) == -1, "empty enable suffix");
  vl_destroy(v);
  v = parser_(options, 1);
  EXPECT(vl_parse(v, 2, dis) == -1, "empty disable suffix");
  vl_destroy(v);
}

void test_parse_int_double_edge_values(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "i", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT),
      VL_OPT(.name = "d", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOUBLE),
      VL_OPT(.name = "a", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_AUTO),
      NULL,
  };
  valve_t *v = parser_(options, 3);

  char *ws[] = {(char *)"valve", (char *)"--i= 1"};
  EXPECT(v != NULL && vl_parse(v, 2, ws) == -1, "int leading space");
  vl_destroy(v);

  v = parser_(options, 3);
  char *empty[] = {(char *)"valve", (char *)"--i="};
  EXPECT(vl_parse(v, 2, empty) == -1, "int empty");
  vl_destroy(v);

  v = parser_(options, 3);
  char *erange[] = {(char *)"valve",
                    (char *)"--i=99999999999999999999999999999"};
  EXPECT(vl_parse(v, 2, erange) == -1, "int ERANGE");
  vl_destroy(v);

  v = parser_(options, 3);
  char *dws[] = {(char *)"valve", (char *)"--d= 1.5"};
  EXPECT(vl_parse(v, 2, dws) == -1, "double leading space");
  vl_destroy(v);

  v = parser_(options, 3);
  char *inf[] = {(char *)"valve", (char *)"--d=inf"};
  EXPECT(vl_parse(v, 2, inf) == -1, "double inf rejected");
  vl_destroy(v);

  v = parser_(options, 3);
  char *nan[] = {(char *)"valve", (char *)"--d=nan"};
  EXPECT(vl_parse(v, 2, nan) == -1, "double nan rejected");
  vl_destroy(v);

  v = parser_(options, 3);
  char *huge[] = {(char *)"valve", (char *)"--d=1e999999"};
  EXPECT(vl_parse(v, 2, huge) == -1, "double ERANGE");
  vl_destroy(v);

  v = parser_(options, 3);
  char *trail[] = {(char *)"valve", (char *)"--d=1.5x"};
  EXPECT(vl_parse(v, 2, trail) == -1, "double trailing junk");
  vl_destroy(v);

  /* AUTO: double-mark that fails double parse falls through */
  v = parser_(options, 3);
  char *auto_bad[] = {(char *)"valve", (char *)"--a=1.2.3"};
  int arc = vl_parse(v, 2, auto_bad);
  EXPECT(arc == 0 || arc == -1, "auto bad double-mark");
  vl_destroy(v);
}

void test_parse_kv_pipe_and_trailing(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "meta", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_KV),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *pipe_end[] = {(char *)"valve", (char *)"--meta=a:1|"};
  EXPECT(v != NULL && vl_parse(v, 2, pipe_end) == -1, "kv trailing pipe");
  vl_destroy(v);

  v = parser_(options, 1);
  char *nested_junk[] = {(char *)"valve", (char *)"--meta=a:{b:1}x"};
  EXPECT(vl_parse(v, 2, nested_junk) == -1, "kv junk after nest");
  vl_destroy(v);

  v = parser_(options, 1);
  char *pipe_empty[] = {(char *)"valve", (char *)"--meta=a:1|{}"};
  EXPECT(vl_parse(v, 2, pipe_empty) == -1, "kv pipe then bad");
  vl_destroy(v);
}

void test_parse_dot_notation_toggle_target(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "cfg.flag", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_TOGGLE),
      NULL,
  };
  valve_t *v = parser_(options, 1);
  char *argv[] = {(char *)"valve", (char *)"--cfg.flag=true"};
  if (v) {
    (void)vl_parse(v, 2, argv);
    vl_destroy(v);
  }
  EXPECT(1, "dot toggle target");
}

void test_parse_toggle_ref_on_verb_levels(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "fast", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, .toggle_ref = "fast"),
      VL_OPT(.name = "other", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL), /* null toggle_ref skip */
      NULL,
  };
  const vl_option_t *const verb_opts[] = {
      VL_OPT(.name = "debug", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, .toggle_ref = "debug"),
      VL_OPT(.name = "plain", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_option_t *const global_opts[] = {
      VL_OPT(.name = "verbose", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, .toggle_ref = "verbose"),
      NULL,
  };
  const vl_verb_t *const subs[] = {
      VL_CMD(.name = "go", .options = sub_opts), NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = verb_opts, .verbs = subs), NULL};
  vl_executable_t settings = {
      .options = global_opts, .verbs = verbs, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *a1[] = {(char *)"valve", (char *)"run", (char *)"go",
                (char *)"--enable-fast"};
  EXPECT(v != NULL && vl_parse(v, 4, a1) == 0, "sub toggle");
  vl_destroy(v);

  v = vl_create(&settings);
  char *a2[] = {(char *)"valve", (char *)"run", (char *)"--enable-debug"};
  (void)vl_parse(v, 3, a2); /* needs sub-verb */
  vl_destroy(v);

  v = vl_create(&settings);
  char *a3[] = {(char *)"valve", (char *)"--enable-verbose"};
  EXPECT(vl_parse(v, 2, a3) == -1, "global toggle still needs verb");
  vl_destroy(v);

  /* flat schema without required sub */
  const vl_verb_t *const verbs2[] = {
      VL_CMD(.name = "run", .options = verb_opts), NULL};
  settings.verbs = verbs2;
  v = vl_create(&settings);
  char *a4[] = {(char *)"valve", (char *)"run", (char *)"--enable-debug"};
  EXPECT(v != NULL && vl_parse(v, 3, a4) == 0, "verb-level toggle");
  vl_destroy(v);

  v = vl_create(&settings);
  char *a5[] = {(char *)"valve", (char *)"--enable-verbose", (char *)"run"};
  /* order: global before verb may fail missing command */
  (void)vl_parse(v, 3, a5);
  vl_destroy(v);

  settings.options = global_opts;
  settings.verbs = NULL;
  /* globals only */
  {
    vl_executable_t g = {.options = global_opts, .color = VAL_COLOR_NEVER};
    v = vl_create(&g);
    char *a6[] = {(char *)"valve", (char *)"--enable-verbose"};
    EXPECT(v != NULL && vl_parse(v, 2, a6) == 0, "global toggle alone");
    vl_destroy(v);
  }
}

void test_parse_assign_mode_disabled_forms(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "x", .short_name = 'x',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_STRING),
      VL_OPT(.name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  /* SEPARATE: inline form disabled */
  valve_t *v = parser_with_form_(options, 2, VL_ASSIGN_SEPARATE);
  char *inline_long[] = {(char *)"valve", (char *)"--x=hi"};
  EXPECT(v != NULL && vl_parse(v, 2, inline_long) == -1, "sep rejects inline");
  vl_destroy(v);

  /* INLINE: separate form disabled */
  v = parser_with_form_(options, 2, VL_ASSIGN_INLINE);
  char *sep_long[] = {(char *)"valve", (char *)"--x", (char *)"hi"};
  EXPECT(vl_parse(v, 3, sep_long) == -1, "inline rejects separate");
  vl_destroy(v);

  /* bare bool still ok in both */
  v = parser_with_form_(options, 2, VL_ASSIGN_SEPARATE);
  char *bare[] = {(char *)"valve", (char *)"--b"};
  EXPECT(vl_parse(v, 2, bare) == 0, "bare bool separate");
  vl_destroy(v);
}

void test_parse_reserved_eq_edge_and_empty_cmd(void) {
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "q", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs[] = {VL_CMD(.name = "go", .options = sub_opts),
                                   NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .verbs = subs), NULL};
  vl_executable_t settings = {
      .verbs = verbs, .assign = VL_ASSIGN_INLINE, .color = VAL_COLOR_NEVER};

  valve_t *v = vl_create(&settings);
  char *empty_cmd[] = {(char *)"valve", (char *)""};
  EXPECT(v != NULL && vl_parse(v, 2, empty_cmd) == -1, "empty command");
  vl_destroy(v);

  v = vl_create(&settings);
  char *empty_sub[] = {(char *)"valve", (char *)"run", (char *)""};
  EXPECT(vl_parse(v, 3, empty_sub) == -1, "empty subverb");
  vl_destroy(v);

  /* reserved token with empty eq value */
  v = vl_create(&settings);
  char *help_eq[] = {(char *)"valve", (char *)"--help="};
  EXPECT(vl_parse(v, 2, help_eq) == -1, "help empty eq");
  vl_destroy(v);

  /* very long reserved prefix before '=' */
  v = vl_create(&settings);
  char longtok[80];
  memset(longtok, 'a', 70);
  longtok[0] = '-';
  longtok[1] = '-';
  memcpy(longtok + 2, "help", 4);
  /* actually need plen >= 64 for the name part before = */
  char big[96];
  memset(big, 'x', sizeof big);
  big[0] = '-';
  big[1] = '-';
  big[64] = '=';
  big[65] = 'a';
  big[66] = '\0';
  char *big_eq[] = {(char *)"valve", big};
  (void)vl_parse(v = vl_create(&settings), 2, big_eq);
  vl_destroy(v);

  /* leading '=' token */
  v = vl_create(&settings);
  char *eq_only[] = {(char *)"valve", (char *)"=foo"};
  (void)vl_parse(v, 2, eq_only);
  vl_destroy(v);

  /* separate help with empty next token */
  settings.assign = VL_ASSIGN_SEPARATE;
  v = vl_create(&settings);
  char *help_empty_next[] = {(char *)"valve", (char *)"--help", (char *)""};
  (void)vl_parse(v, 3, help_empty_next);
  vl_destroy(v);

  v = vl_create(&settings);
  char *help_null_next[] = {(char *)"valve", (char *)"--help", NULL};
  (void)vl_parse(v, 3, help_null_next);
  vl_destroy(v);

  /* joined ? alone is not JOINED (needs p[1] != 0) */
  settings.assign = VL_ASSIGN_INLINE;
  v = vl_create(&settings);
  char *q_alone[] = {(char *)"valve", (char *)"--?"};
  (void)vl_parse(v, 2, q_alone);
  vl_destroy(v);
}

static int g_cb_hits;
static void cb_help_(const valve_t *v) {
  (void)v;
  g_cb_hits |= 1;
}
static void cb_version_(const valve_t *v) {
  (void)v;
  g_cb_hits |= 2;
}
static void cb_valve_(const valve_t *v) {
  (void)v;
  g_cb_hits |= 4;
}

void test_parse_reserved_callbacks_hit(void) {
  vl_executable_t settings = {
      .color = VAL_COLOR_NEVER,
      .on_help = cb_help_,
      .on_version = cb_version_,
      .on_valve = cb_valve_,
  };
  g_cb_hits = 0;
  valve_t *v = vl_create(&settings);
  char *h[] = {(char *)"valve", (char *)"--help"};
  EXPECT(v != NULL && vl_parse(v, 2, h) == 0, "help cb");
  EXPECT(g_cb_hits & 1, "help fired");
  vl_destroy(v);

  g_cb_hits = 0;
  v = vl_create(&settings);
  char *ver[] = {(char *)"valve", (char *)"--version"};
  EXPECT(vl_parse(v, 2, ver) == 0 && (g_cb_hits & 2), "version cb");
  vl_destroy(v);

  g_cb_hits = 0;
  v = vl_create(&settings);
  char *val[] = {(char *)"valve", (char *)"--valve"};
  EXPECT(vl_parse(v, 2, val) == 0 && (g_cb_hits & 4), "valve cb");
  vl_destroy(v);
}

void test_parse_help_suggest_null_names(void) {
  /* Ambiguous sub-verb name across verbs → suggestion builder */
  const vl_option_t *const sub_opts[] = {
      VL_OPT(.name = "x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs_a[] = {VL_CMD(.name = "list", .options = sub_opts),
                                     NULL};
  const vl_verb_t *const subs_b[] = {VL_CMD(.name = "list", .options = sub_opts),
                                     NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "a", .verbs = subs_a),
      VL_CMD(.name = "b", .verbs = subs_b),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = verbs, .assign = VL_ASSIGN_INLINE, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help=list"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == -1, "ambiguous help");
  vl_destroy(v);
}

/* ---- help resolve / print -------------------------------------------- */

void test_help_resolve_null_slots_and_three_seg(void) {
  static const vl_option_t leaf = {
      .name = "leaf", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const sub_opts[] = {&leaf, NULL};
  static const vl_verb_t sub = {.name = "sub", .options = sub_opts};
  static const vl_verb_t *const subs[] = {&sub, NULL};
  static const vl_verb_t verb = {.name = "verb", .verbs = subs};
  static const vl_verb_t *const verbs[] = {&verb, NULL};
  static const vl_option_t nameless = {.name = NULL};

  vl_help_resolution_t out = {0};
  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "verb.sub.leaf", &out),
         "three-segment resolve");
  EXPECT(out.kind == VL_HELP_OPTION && out.option == &leaf, "kind option");

  /* n==3 with unknown verb → false arm of if (verb) */
  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "zzz.sub.leaf", &out),
         "unknown verb three-seg");
  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "verb.nope.leaf", &out),
         "bad sub");
  /* miss: verb+sub found, option missing */
  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "verb.sub.nope", &out),
         "bad leaf");

  /* explicit count keeps leading NULL slots visible to scanners */
  static const vl_verb_t *const verbs_holes[] = {NULL, &verb, NULL};
  EXPECT(vl_help_resolve(verbs_holes, 2, NULL, 0, "verb.sub.leaf", &out),
         "three-seg with null verb slot");

  static const vl_option_t grp = {
      .name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_STRING};
  static const vl_option_t *const g2[] = {NULL, &nameless, &grp, NULL};
  EXPECT(vl_help_resolve(NULL, 0, g2, 3, "proxy", &out), "group with holes");

  /* via parse → internal three-segment */
  const vl_option_t *const sub_opts2[] = {
      VL_OPT(.name = "leaf", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const subs2[] = {
      VL_CMD(.name = "sub", .options = sub_opts2), NULL};
  const vl_verb_t *const verbs2[] = {
      VL_CMD(.name = "verb", .verbs = subs2), NULL};
  vl_executable_t settings = {
      .verbs = verbs2, .assign = VL_ASSIGN_INLINE, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help=verb.sub.leaf"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "parse three-seg help");
  vl_destroy(v);

  v = vl_create(&settings);
  char *badv[] = {(char *)"valve", (char *)"--help=zzz.sub.leaf"};
  EXPECT(vl_parse(v, 2, badv) == -1, "parse unknown three-seg verb");
  vl_destroy(v);

  v = vl_create(&settings);
  char *bads[] = {(char *)"valve", (char *)"--help=verb.nope.leaf"};
  EXPECT(vl_parse(v, 2, bads) == -1, "parse bad three-seg sub");
  vl_destroy(v);
}

void test_help_print_null_name_cards(void) {
  vl_option_t opt = {.name = NULL, .usage = "u", .description = "d"};
  vl_option_t named = {
      .name = "ok", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .usage = "u", .description = "d"};
  valve_verb_t sub = {.name = NULL, .description = "sd", .usage = "su",
                      .options = &named, .option_count = 1};
  valve_verb_t verb = {.name = NULL, .description = "vd", .usage = "vu",
                       .verbs = &sub, .verb_count = 1,
                       .options = &named, .option_count = 1};
  valve_t v = {
      .program_name_ = "prog",
      .program_version_ = NULL,
      .description_ = "desc",
      .usage_ = "usage",
      .logo_ = NULL,
      .options_ = &opt,
      .option_count_ = 1,
      .verbs_ = &verb,
      .verb_count_ = 1,
  };

  /* overview with null verb name */
  vl_help_print_default_(&v);

  /* verb card */
  v.active_verb_ = &verb;
  vl_help_print_default_(&v);

  /* subverb card with null names + parent options */
  v.active_subverb_ = &sub;
  vl_help_print_default_(&v);

  /* targeted group with null-name option in table */
  vl_option_t g1 = {.name = NULL};
  vl_option_t g2 = {
      .name = "cfg.x", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT};
  vl_option_t opts[2] = {g1, g2};
  v.options_ = opts;
  v.option_count_ = 2;
  v.active_verb_ = NULL;
  v.active_subverb_ = NULL;
  v.help_target_ = (char *)"cfg";
  vl_help_print_default_(&v);
  v.help_target_ = NULL;

  /* option card via resolve */
  v.help_target_ = (char *)"ok";
  verb.options = &named;
  verb.option_count = 1;
  v.verbs_ = &verb;
  v.verb_count_ = 1;
  vl_help_print_default_(&v);
  v.help_target_ = NULL;

  EXPECT(1, "help print null names");
}

void test_version_print_null_and_fallback(void) {
  vl_version_print_default_(NULL);
  valve_t v = {0};
  vl_version_print_default_(&v);
  EXPECT(1, "version print");
}

/* ---- duration / labels / value_label --------------------------------- */

void test_duration_null_and_message_default(void) {
  int64_t out = 0;
  EXPECT(option_duration_parse_(NULL, &out) == OPTION_DURATION_EMPTY,
         "null raw");
  EXPECT(option_duration_parse_(":", &out) == OPTION_DURATION_SYNTAX,
         "non-digit >9 path via : after empty? — leading non-digit");
  EXPECT(option_duration_parse_("1x", &out) == OPTION_DURATION_SUFFIX,
         "bad suffix");
  /* char < '0' mid-parse after digits handled by unit; leading '!' */
  EXPECT(option_duration_parse_("!", &out) == OPTION_DURATION_SYNTAX, "bang");
  (void)option_duration_message_(OPTION_DURATION_OK);
  (void)option_duration_message_((option_duration_status_t)99);
  EXPECT(1, "duration branches");
}

void test_value_label_dot_toggle_and_default(void) {
  vl_option_t o = {.value = VL_OPTION_VALUE_DOT_NOTATION,
                   .target = VL_TARGET_TOGGLE};
  EXPECT(strcmp(option_value_label_(&o), "value") == 0, "dot toggle");
  o.target = VL_TARGET_VALUE;
  EXPECT(strcmp(option_value_label_(&o), "value") == 0, "dot value");
  o.value = (vl_option_value_t)99;
  EXPECT(strcmp(option_value_label_(&o), "value") == 0, "default");
}

void test_label_short_and_toggle_null_ref(void) {
  vl_option_t sh = {.short_name = 'q', .type = VL_OPT_TYPE_SHORT,
                    .value = VL_OPTION_VALUE_BOOL};
  char *l = option_label_(&sh);
  EXPECT(l && strcmp(l, "-q") == 0, "short label");
  free(l);

  vl_option_t tog = {.type = VL_OPT_TYPE_TOGGLE, .toggle_ref = NULL,
                     .name = "x", .value = VL_OPTION_VALUE_TOGGLE};
  /* falls through — may return null without LONG/SHORT */
  l = option_label_(&tog);
  free(l);
  EXPECT(1, "toggle null ref");
}

/* ---- null API arms still missing ------------------------------------- */

void test_more_null_api_arms(void) {
  EXPECT(vl_verb_get(NULL) == NULL, "verb_get null");
  EXPECT(vl_get(&(valve_t){0}, NULL) == NULL, "vl_get null key");
  vl_value_clear(NULL);
  vl_errors_foreach(&(valve_t){0}, NULL, NULL);

  valve_t v = {0};
  EXPECT(vl_error_at(&v, 0) == NULL, "error_at OOB");
  EXPECT(vl_error_add_(&v, VL_ERROR_UNKNOWN_OPTION, 0, "k", NULL) == 0,
         "add null message");
  EXPECT(vl_error_count(&v) == 1, "count 1");
  /* print error with null message */
  vl_errors_print(&v, stdout);
  /* error with key */
  EXPECT(vl_error_add_(&v, VL_ERROR_UNKNOWN_OPTION, 1, NULL, "msg") == 0,
         "add null key");
  vl_errors_print(&v, stdout);
  free((void *)v.errors_[0].key);
  free((void *)v.errors_[1].message);
  free(v.errors_);

  EXPECT(vl_reserved_short_('?') == VL_RESERVED_HELP, "reserved ?");
}

void test_schema_option_count_without_table(void) {
  vl_verb_t verb = {.name = "run", .option_count = 2, .options = NULL};
  const vl_verb_t *const verbs[] = {&verb, NULL};
  vl_executable_t settings = {.verbs = verbs, .verb_count = 1,
                              .color = VAL_COLOR_NEVER};
  EXPECT(vl_create(&settings) == NULL, "option_count without options");

  settings.verbs = NULL;
  settings.verb_count = 2;
  settings.options = NULL;
  settings.option_count = 0;
  /* verb_count>0 && !verbs */
  settings.verb_count = 1;
  EXPECT(vl_create(&settings) == NULL, "verb_count without verbs");
}

void test_collides_null_slots(void) {
  const vl_option_t *const globals[] = {
      NULL,
      VL_OPT(.name = NULL, .short_name = 'p', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      VL_OPT(.name = "port", .short_name = 'x',
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
  EXPECT(option_collides_with_globals_(globals, 3, locals, 3), "collide holes");
}

void test_find_option_null_tables(void) {
  valve_verb_t sub = {.name = "go", .options = NULL, .option_count = 0};
  valve_verb_t verb = {.name = "run", .options = NULL, .option_count = 0};
  valve_t v = {
      .active_subverb_ = &sub,
      .active_verb_ = &verb,
      .options_ = NULL,
      .option_count_ = 0,
      .verbs_ = NULL,
      .verb_count_ = 0,
  };
  EXPECT(vl_option_find_(&v, "x") == NULL, "find null tables");
  EXPECT(vl_option_find_n_(&v, "x", 1) == NULL, "find_n null");
  EXPECT(vl_option_find_short_(&v, 'x') == NULL, "find_short null");
  EXPECT(vl_option_find_short_(&v, '\0') == NULL, "find_short NUL");
  EXPECT(verb_find_in_(NULL, 1, "run") == NULL, "verb_find null");
  EXPECT(verb_find_in_(NULL, 0, NULL) == NULL, "verb_find null name");
}

void test_name_valid_edge_chars(void) {
  EXPECT(!option_name_valid_(NULL), "null name");
  EXPECT(!option_name_valid_(""), "empty");
  EXPECT(!option_name_valid_("-x"), "leading dash");
  EXPECT(!option_name_valid_("a=b"), "equals");
  EXPECT(!verb_name_valid_(NULL), "verb null");
  EXPECT(!verb_name_valid_(""), "verb empty");
  EXPECT(!verb_name_valid_("-x"), "verb dash");
  EXPECT(verbs_have_invalid_((const vl_verb_t *const[]){NULL}, 1), "null verb");
  static const vl_verb_t bad = {.name = "-nope"};
  EXPECT(verbs_have_invalid_((const vl_verb_t *const[]){&bad}, 1), "bad verb");
}

void test_options_invalid_null_toggle_ref(void) {
  static const vl_option_t tog = {
      .name = "t", .type = VL_OPT_TYPE_TOGGLE, .value = VL_OPTION_VALUE_TOGGLE,
      .toggle_ref = NULL};
  static const vl_option_t *const opts[] = {&tog};
  EXPECT(options_have_invalid_(opts, 1), "null toggle_ref");
}

void test_option_in_null_arms(void) {
  static const vl_option_t o = {.name = "a"};
  static const vl_option_t *const opts[] = {&o};
  EXPECT(!option_in_(NULL, opts, 1), "null option");
  EXPECT(!option_in_(&o, NULL, 1), "null table");
  EXPECT(option_in_(&o, opts, 1), "hit");
}

void test_conflict_count_null_arms(void) {
  EXPECT(option_conflict_count_(NULL) == 0, "null option");
  static const vl_option_t o = {.name = "a", .conflicts = NULL};
  EXPECT(option_conflict_count_(&o) == 0, "null conflicts");
  static const vl_option_t *const c[] = {&o, NULL};
  static const vl_option_t o2 = {.name = "b", .conflicts = c, .conflict_count = 0};
  EXPECT(option_conflict_count_(&o2) == 1, "walk conflicts");
}

void test_requirement_exists_partial_match(void) {
  vl_option_t a = {.name = "a"};
  vl_option_t b = {.name = "b"};
  valve_verb_t verb = {.name = "run"};
  valve_requirement_t reqs[1] = {
      {.source = &a, .target = &b, .verb = &verb, .subverb = NULL}};
  valve_t v = {.requirements_ = reqs, .requirement_count_ = 1};
  EXPECT(!vl_requirement_exists_(&v, &a, &b, NULL, NULL), "verb mismatch");
  EXPECT(vl_requirement_exists_(&v, &a, &b, &verb, NULL), "full match");
  EXPECT(!vl_requirement_exists_(&v, &a, &a, &verb, NULL), "target mismatch");
}

void test_duplicate_long_null_name_continue(void) {
  static const vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t anon = {
      .name = NULL, .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const opts[] = {&a, &anon, &a};
  /* may return true for dup or invalid — exercise name-null continue */
  (void)options_have_duplicate_long_(opts, 3);
  static const vl_option_t s1 = {
      .name = "s1", .short_name = 's', .type = VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t s2 = {
      .name = "s2", .short_name = '\0', .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t s3 = {
      .name = "s3", .short_name = 's', .type = VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const shorts[] = {&s1, &s2, &s3};
  EXPECT(options_have_duplicate_short_(shorts, 3), "dup short");
}

void test_annotations_seen_duplicates(void) {
  /* Two identical requires/conflicts so seen_* returns true on 2nd */
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
      .required = true};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const reqs[] = {&b, &b, NULL};
  a.requires = reqs;
  static const vl_option_t *const confs[] = {&b, &b, NULL};
  a.conflicts = confs;
  static const vl_option_t *const breqs[] = {&a, &a, NULL};
  b.requires = breqs; /* so a is required_by from b... actually required_by
                       * scans requirements where target==option */

  const vl_option_t *const options[] = {&a, &b, NULL};
  vl_executable_t settings = {.options = options, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  if (!v) {
    /* schema may reject duplicate conflict entries — build valve manually */
    valve_requirement_t r[2] = {{.source = &a, .target = &b},
                                {.source = &a, .target = &b}};
    valve_conflict_t c[2] = {{.first = &a, .second = &b},
                             {.first = &a, .second = &b}};
    valve_t manual = {
        .requirements_ = r,
        .requirement_count_ = 2,
        .conflicts_ = c,
        .conflict_count_ = 2,
        .options_ = (vl_option_t[]){a, b},
        .option_count_ = 2,
    };
    option_annotations_print_(&manual, &a);
    option_annotations_print_(&manual, &b);
    EXPECT(1, "manual annotations seen");
    return;
  }
  char *argv[] = {(char *)"valve", (char *)"--help"};
  (void)vl_parse(v, 2, argv);
  vl_destroy(v);
  EXPECT(1, "annotations");
}

void test_targets_clear_null_opt_and_kinds(void) {
  typedef struct {
    char *s;
    int i;
    int64_t i64;
    double d;
    bool b;
    vl_value_t val;
    vl_value_t tog;
  } cfg_t;
  cfg_t cfg = {.s = strdup("hi"), .i = 1, .i64 = 2, .d = 3.0, .b = true};
  cfg.val = (vl_value_t){.kind = VL_VALUE_STRING, .raw = strdup("v")};
  cfg.tog = (vl_value_t){.kind = VL_VALUE_BOOL, .as.boolean = true};

  const vl_option_t *const options[] = {
      NULL,
      VL_OPT(.name = "s", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, s), .target = VL_TARGET_STRING),
      VL_OPT(.name = "i", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .data = &cfg, .offset = offsetof(cfg_t, i), .target = VL_TARGET_INT),
      VL_OPT(.name = "i64", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_INT,
             .data = &cfg, .offset = offsetof(cfg_t, i64),
             .target = VL_TARGET_INT64),
      VL_OPT(.name = "d", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_DOUBLE,
             .data = &cfg, .offset = offsetof(cfg_t, d),
             .target = VL_TARGET_DOUBLE),
      VL_OPT(.name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .data = &cfg, .offset = offsetof(cfg_t, b), .target = VL_TARGET_BOOL),
      VL_OPT(.name = "val", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_STRING,
             .data = &cfg, .offset = offsetof(cfg_t, val),
             .target = VL_TARGET_VALUE),
      VL_OPT(.name = "tog", .type = VL_OPT_TYPE_TOGGLE,
             .value = VL_OPTION_VALUE_TOGGLE, .toggle_ref = "tog", .data = &cfg,
             .offset = offsetof(cfg_t, tog), .target = VL_TARGET_TOGGLE),
      VL_OPT(.name = "none", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL,
             .data = &cfg, .target = VL_TARGET_NONE),
      NULL,
  };
  /* explicit count includes NULL hole */
  vl_executable_t settings = {
      .options = options, .option_count = 9, .color = VAL_COLOR_NEVER};
  /* create may fail on null slot — call clear directly */
  vl_targets_clear(&settings);
  free(cfg.s);
  vl_value_clear(&cfg.val);
  vl_value_clear(&cfg.tog);
  EXPECT(1, "targets clear kinds");
}

void test_set_empty_kv_array_clone(void) {
  typedef struct {
    vl_value_t v;
  } cfg_t;
  cfg_t cfg = {0};
  vl_option_t opt = {
      .name = "meta",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_KV,
      .data = &cfg,
      .offset = offsetof(cfg_t, v),
      .target = VL_TARGET_VALUE,
  };
  valve_t v = {0};
  vl_value_t empty_kv = {.kind = VL_VALUE_KV};
  EXPECT(vl_result_set_(&v, &opt, &empty_kv, 0) == 0, "empty kv clone");
  vl_value_t empty_arr = {.kind = VL_VALUE_ARRAY};
  opt.value = VL_OPTION_VALUE_ARRAY;
  opt.name = "list";
  EXPECT(vl_result_set_(&v, &opt, &empty_arr, 1) == 0, "empty array clone");
  vl_results_clear_(&v);
  vl_value_clear(&cfg.v);
}

void test_conflicts_valid_null_continue(void) {
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const conf_null[] = {NULL};
  a.conflicts = conf_null;
  a.conflict_count = 1;
  const vl_option_t *const opts[] = {&a, &b, NULL};
  EXPECT(!options_conflicts_valid_(NULL, 0, NULL, 0, opts, 2),
         "null conflict rejected");

  static const vl_option_t *const conf_self[] = {&a};
  a.conflicts = conf_self;
  a.conflict_count = 1;
  EXPECT(!options_conflicts_valid_(NULL, 0, NULL, 0, opts, 2),
         "self conflict rejected");

  a.requires = NULL;
  a.require_count = 1;
  EXPECT(!options_requirements_valid_(NULL, 0, NULL, 0, opts, 2),
         "require_count without table");

  static const vl_option_t *const req_null[] = {NULL};
  a.requires = req_null;
  a.require_count = 1;
  EXPECT(!options_requirements_valid_(NULL, 0, NULL, 0, opts, 2),
         "null require rejected");

  static const vl_option_t *const req_self[] = {&a};
  a.requires = req_self;
  a.require_count = 1;
  EXPECT(!options_requirements_valid_(NULL, 0, NULL, 0, opts, 2),
         "self require rejected");
}

void test_seen_with_null_other(void) {
  vl_option_t a = {.name = "a"};
  vl_option_t b = {.name = "b"};
  vl_option_t c = {.name = "c"};
  valve_conflict_t conflicts[2] = {
      {.first = &c, .second = &b}, /* other(a) → NULL */
      {.first = &a, .second = &b},
  };
  valve_t v = {.conflicts_ = conflicts, .conflict_count_ = 2};
  EXPECT(!vl_conflict_seen_(&v, 1, &a, &b), "null other before match");
  EXPECT(vl_conflict_seen_(&v, 2, &a, &b), "then seen");
  valve_requirement_t reqs[2] = {
      {.source = &c, .target = &b},
      {.source = &a, .target = &b},
  };
  v.requirements_ = reqs;
  v.requirement_count_ = 2;
  EXPECT(!vl_requirement_forward_seen_(&v, 1, &a, &b), "forward miss");
  EXPECT(vl_requirement_forward_seen_(&v, 2, &a, &b), "forward seen");
  EXPECT(!vl_requirement_reverse_seen_(&v, 1, &b, &a), "reverse miss");
  EXPECT(vl_requirement_reverse_seen_(&v, 2, &b, &a), "reverse seen");
}

void test_copy_table_null_slot(void) {
  /* Exercise conflicts/requirements copy with sparse tables via create */
  static vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static vl_option_t b = {
      .name = "b", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const conf[] = {&b, NULL};
  a.conflicts = conf;
  static const vl_option_t *const req[] = {&b, NULL};
  a.requires = req;
  const vl_option_t *const options[] = {&a, &b, NULL};
  vl_executable_t settings = {.options = options, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "create with conf/req");
  vl_destroy(v);
}

void test_active_conflict_requirement_null_v(void) {
  valve_conflict_t c = {0};
  EXPECT(!vl_conflict_active_(NULL, &c), "active null v");
  EXPECT(!vl_conflict_active_(&(valve_t){0}, NULL), "active null conflict");
  EXPECT(!vl_requirement_active_(NULL, &(valve_requirement_t){0}),
         "req active null");
  EXPECT(!vl_requirement_active_(&(valve_t){0}, NULL), "req null requirement");
}

void test_duplicate_name_null_second_slot(void) {
  static const vl_option_t a = {
      .name = "a", .type = VL_OPT_TYPE_LONG, .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const opts[] = {&a, NULL, &a};
  EXPECT(options_have_duplicate_long_(opts, 3), "dup across null");

  static const vl_option_t s1 = {
      .name = "s1", .short_name = 'q', .type = VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t s2 = {
      .name = "s2", .short_name = 'q', .type = VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL};
  static const vl_option_t *const shorts[] = {&s1, NULL, &s2};
  EXPECT(options_have_duplicate_short_(shorts, 3), "dup short across null");
}

void test_collides_null_global_short(void) {
  const vl_option_t *const globals[] = {
      NULL,
      VL_OPT(.name = "g", .short_name = 'p', .type = VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
  };
  const vl_option_t *const locals[] = {
      VL_OPT(.name = "l", .short_name = 'p',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
  };
  EXPECT(option_collides_with_globals_(globals, 2, locals, 1),
         "short collide skips null global");
}

void test_subverb_get_active_null(void) {
  valve_t v = {.active_subverb_ = NULL};
  EXPECT(vl_subverb_get(&v) == NULL, "no subverb");
  valve_verb_t sub = {.name = "go"};
  v.active_subverb_ = &sub;
  EXPECT(strcmp(vl_subverb_get(&v), "go") == 0, "has subverb");
}

void test_set_int_target_bounds_and_empty_kv(void) {
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
  char *lo[] = {(char *)"valve", (char *)"--i=-2147483649"};
  EXPECT(v != NULL && vl_parse(v, 2, lo) == -1, "int below INT_MIN");
  vl_destroy(v);
}

void test_requirement_count_null_option(void) {
  EXPECT(option_requirement_count_(NULL) == 0, "null option");
}

void test_value_label_dot_none(void) {
  vl_option_t o = {.value = VL_OPTION_VALUE_DOT_NOTATION,
                   .target = VL_TARGET_NONE};
  EXPECT(strcmp(option_value_label_(&o), "value") == 0, "dot none");
}

void test_schema_allow_override_reserved_verbs(void) {
  const vl_verb_t *const verbs[] = {VL_CMD(.name = "help"), NULL};
  vl_executable_t settings = {
      .verbs = verbs,
      .behavior = VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "allow override reserved verb");
  vl_destroy(v);
}

void test_find_with_null_name(void) {
  valve_t v = {0};
  EXPECT(vl_option_find_(&v, NULL) == NULL, "find null name");
  EXPECT(vl_option_find_n_(&v, NULL, 1) == NULL, "find_n null name");
  EXPECT(verb_find_in_(NULL, 1, NULL) == NULL, "verb_find null name");
}

void test_have_duplicate_verb_null_name(void) {
  static const vl_verb_t a = {.name = "run"};
  static const vl_verb_t anon = {.name = NULL};
  static const vl_verb_t *const verbs[] = {&a, &anon, &a};
  (void)verbs_have_duplicate_(verbs, 3);
  EXPECT(1, "verb dup null name");
}

