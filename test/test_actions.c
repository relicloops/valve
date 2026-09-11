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

#include "fault_alloc.h"
#include "private.h"

#include <unistd.h>

void setUp(void) { vl_test_alloc_reset(); }
void tearDown(void) {
  vl_test_alloc_reset();
  vl_color_support_reset();
}

/* ---- fixtures --------------------------------------------------------- */

static int update_calls_ = 0;
static int telemetry_calls_ = 0;
static const char *fired_name_in_run_ = NULL;
static bool reserved_in_run_ = false;

static void run_update_(const valve_t *v) {
  ++update_calls_;
  fired_name_in_run_ = vl_action_fired(v);
  reserved_in_run_ = vl_reserved_fired(v);
}

static void run_telemetry_(const valve_t *v) {
  (void)v;
  ++telemetry_calls_;
}

static void reset_counters_(void) {
  update_calls_ = 0;
  telemetry_calls_ = 0;
  fired_name_in_run_ = NULL;
  reserved_in_run_ = false;
}

#define VL_ACT(...) (&(const vl_executable_action_t){__VA_ARGS__})

static const vl_executable_action_t update_bool_ = {
    .option = {.name = "update", .short_name = 'u',
               .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
               .value = VL_OPTION_VALUE_BOOL,
               .description = "replace this binary"},
    .run = run_update_,
};

static const vl_executable_action_t telemetry_toggle_ = {
    .option = {.name = "telemetry", .toggle_ref = "telemetry",
               .type = VL_OPT_TYPE_TOGGLE, .value = VL_OPTION_VALUE_TOGGLE,
               .description = "opt in or out of telemetry"},
    .run = run_telemetry_,
};

static const vl_option_t *const verb_opts_[] = {
    VL_OPT(.name = "count", .type = VL_OPT_TYPE_LONG,
           .value = VL_OPTION_VALUE_INT),
    NULL,
};

static const vl_verb_t *const verbs_[] = {
    VL_CMD(.name = "universe", .options = verb_opts_),
    NULL,
};

static const vl_option_t *const globals_[] = {
    VL_OPT(.name = "verbose", .short_name = 'V',
           .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
           .value = VL_OPTION_VALUE_BOOL),
    NULL,
};

static valve_t *actions_parser_(const vl_executable_action_t *const *actions,
                                vl_assign_t assign, bool with_verbs) {
  vl_executable_t settings = {
      .assign = assign,
      .behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
      .options = globals_,
      .verbs = with_verbs ? verbs_ : NULL,
      .actions = actions,
      .color = VAL_COLOR_NEVER,
  };
  return vl_create(&settings);
}

/* ---- schema gate ------------------------------------------------------ */

void test_actions_require_behavior_flag(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  vl_executable_t settings = {.actions = actions};
  EXPECT(vl_create(&settings) == NULL, "actions without flag are refused");
}

void test_flag_without_actions_is_inert(void) {
  vl_executable_t settings = {.behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
                              .options = globals_};
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "flag alone creates");
  char *argv[] = {(char *)"prog", (char *)"--update"};
  EXPECT(vl_parse(v, 2, argv) == -1, "--update is unknown without a table");
  EXPECT(vl_action_fired(v) == NULL, "nothing fired");
  vl_destroy(v);
}

void test_action_count_without_table_is_refused(void) {
  vl_executable_t settings = {.behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
                              .action_count = 1};
  EXPECT(vl_create(&settings) == NULL, "count without table");
}

void test_action_count_helper(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_,
                                                   &telemetry_toggle_, NULL};
  EXPECT(action_count_(NULL, 0) == 0, "null table");
  EXPECT(action_count_(actions, 0) == 2, "null-terminated");
  EXPECT(action_count_(actions, 1) == 1, "explicit count");
}

static bool creates_(const vl_executable_action_t *action) {
  const vl_executable_action_t *const actions[] = {action, NULL};
  vl_executable_t settings = {.behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
                              .options = globals_,
                              .actions = actions,
                              .action_count = 1};
  valve_t *v = vl_create(&settings);
  bool ok = v != NULL;
  vl_destroy(v);
  return ok;
}

void test_action_invalid_shapes_are_refused(void) {
  EXPECT(!creates_(NULL), "null slot");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL})),
         "missing run");
  EXPECT(!creates_(VL_ACT(.option = {.name = "", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL},
                          .run = run_update_)),
         "invalid option name");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL,
                                     .required = true},
                          .run = run_update_)),
         "required");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_STRING,
                                     .repeat = VL_OPTION_REPEAT_ARRAY},
                          .run = run_update_)),
         "repeat");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL,
                                     .conflicts = globals_},
                          .run = run_update_)),
         "conflicts");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL,
                                     .conflict_count = 1},
                          .run = run_update_)),
         "conflict_count");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL,
                                     .requires = globals_},
                          .run = run_update_)),
         "requires");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL,
                                     .require_count = 1},
                          .run = run_update_)),
         "require_count");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x",
                                     .value = VL_OPTION_VALUE_COMMAND},
                          .run = run_update_)),
         "command value");
  EXPECT(creates_(VL_ACT(.option = {.name = "x", .type = VL_OPT_TYPE_LONG,
                                    .value = VL_OPTION_VALUE_BOOL},
                         .run = run_update_)),
         "minimal valid action");
}

void test_action_name_collisions_are_refused(void) {
  EXPECT(!creates_(VL_ACT(.option = {.name = "help", .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL},
                          .run = run_update_)),
         "reserved long name");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .short_name = 'h',
                                     .type = VL_OPT_TYPE_LONG |
                                             VL_OPT_TYPE_SHORT,
                                     .value = VL_OPTION_VALUE_BOOL},
                          .run = run_update_)),
         "reserved short name");
  EXPECT(!creates_(VL_ACT(.option = {.name = "verbose",
                                     .type = VL_OPT_TYPE_LONG,
                                     .value = VL_OPTION_VALUE_BOOL},
                          .run = run_update_)),
         "collides with global long");
  EXPECT(!creates_(VL_ACT(.option = {.name = "x", .short_name = 'V',
                                     .type = VL_OPT_TYPE_LONG |
                                             VL_OPT_TYPE_SHORT,
                                     .value = VL_OPTION_VALUE_BOOL},
                          .run = run_update_)),
         "collides with global short");

  /* override flag does not extend to actions */
  const vl_executable_action_t *const actions[] = {
      VL_ACT(.option = {.name = "version", .type = VL_OPT_TYPE_LONG,
                        .value = VL_OPTION_VALUE_BOOL},
             .run = run_update_),
      NULL};
  vl_executable_t settings = {
      .behavior = VL_BEHAVIOR_ACCEPT_NO_VERB |
                  VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED,
      .actions = actions};
  EXPECT(vl_create(&settings) == NULL, "override flag ignored for actions");
}

void test_duplicate_actions_are_refused(void) {
  const vl_executable_action_t *const by_name[] = {
      &update_bool_,
      VL_ACT(.option = {.name = "update", .type = VL_OPT_TYPE_LONG,
                        .value = VL_OPTION_VALUE_BOOL},
             .run = run_update_),
      NULL};
  vl_executable_t settings = {.behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
                              .actions = by_name};
  EXPECT(vl_create(&settings) == NULL, "duplicate long name");

  const vl_executable_action_t *const by_short[] = {
      &update_bool_,
      VL_ACT(.option = {.name = "other", .short_name = 'u',
                        .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
                        .value = VL_OPTION_VALUE_BOOL},
             .run = run_update_),
      NULL};
  settings.actions = by_short;
  EXPECT(vl_create(&settings) == NULL, "duplicate short name");

  const vl_executable_action_t *const distinct[] = {&update_bool_,
                                                    &telemetry_toggle_, NULL};
  settings.actions = distinct;
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "distinct actions create");
  vl_destroy(v);
}

/* ---- parsing ---------------------------------------------------------- */

void test_long_bool_action_fires_without_verb(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, true);
  reset_counters_();
  char *argv[] = {(char *)"prog", (char *)"--update"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "parse ok");
  EXPECT(update_calls_ == 1, "run called once");
  EXPECT(reserved_in_run_, "reserved fired inside run");
  EXPECT(fired_name_in_run_ && strcmp(fired_name_in_run_, "update") == 0,
         "fired name visible inside run");
  EXPECT(vl_reserved_fired(v), "reserved fired after parse");
  EXPECT(strcmp(vl_action_fired(v), "update") == 0, "fired name after parse");
  EXPECT(vl_verb_get(v) == NULL, "no verb selected");
  const vl_value_t *val = vl_get(v, "update");
  EXPECT(val && val->kind == VL_VALUE_BOOL && val->as.boolean,
         "value stored under the action name");
  vl_destroy(v);
}

void test_short_action_fires(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, false);
  reset_counters_();
  char *argv[] = {(char *)"prog", (char *)"-u"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "short parse ok");
  EXPECT(update_calls_ == 1, "run called");
  vl_destroy(v);
}

void test_toggle_action_fires(void) {
  const vl_executable_action_t *const actions[] = {&telemetry_toggle_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, true);
  reset_counters_();
  char *on[] = {(char *)"prog", (char *)"--enable-telemetry"};
  EXPECT(v != NULL && vl_parse(v, 2, on) == 0, "enable parses");
  const vl_value_t *val = vl_get(v, "telemetry");
  EXPECT(val && val->as.boolean, "enable stored true");
  char *off[] = {(char *)"prog", (char *)"--disable-telemetry"};
  EXPECT(vl_parse(v, 2, off) == 0, "disable parses");
  val = vl_get(v, "telemetry");
  EXPECT(val && !val->as.boolean, "disable stored false");
  EXPECT(telemetry_calls_ == 2, "run called twice");
  vl_destroy(v);
}

void test_action_with_string_value_populates_target(void) {
  char *tag = NULL;
  const vl_executable_action_t *const actions[] = {
      VL_ACT(.option = {.name = "update", .type = VL_OPT_TYPE_LONG,
                        .value = VL_OPTION_VALUE_STRING,
                        .target = VL_TARGET_STRING, .data = &tag},
             .run = run_update_),
      NULL};
  vl_executable_t settings = {.behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
                              .actions = actions,
                              .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  reset_counters_();
  char *argv[] = {(char *)"prog", (char *)"--update=v7.4.0"};
  EXPECT(v != NULL && vl_parse(v, 2, argv) == 0, "inline value parses");
  EXPECT(tag && strcmp(tag, "v7.4.0") == 0, "target populated before run");
  EXPECT(update_calls_ == 1, "run called");

  char *bare[] = {(char *)"prog", (char *)"--update"};
  EXPECT(vl_parse(v, 2, bare) == -1, "string action needs a value");
  EXPECT(vl_action_fired(v) == NULL, "fired name cleared on failure");

  vl_targets_clear(&settings);
  EXPECT(tag == NULL, "targets_clear releases action string");
  vl_destroy(v);
}

void test_action_separate_value(void) {
  int level = 0;
  const vl_executable_action_t *const actions[] = {
      VL_ACT(.option = {.name = "level", .short_name = 'l',
                        .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
                        .value = VL_OPTION_VALUE_INT,
                        .target = VL_TARGET_INT, .data = &level,
                        .has_int_max = true, .int_max = 5},
             .run = run_update_),
      NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_SEPARATE, true);
  reset_counters_();
  char *argv[] = {(char *)"prog", (char *)"-l", (char *)"3"};
  EXPECT(v != NULL && vl_parse(v, 3, argv) == 0, "separate value parses");
  EXPECT(level == 3 && update_calls_ == 1, "int target set and run called");

  char *big[] = {(char *)"prog", (char *)"--level", (char *)"9"};
  EXPECT(vl_parse(v, 3, big) == -1, "bounds still enforced");
  EXPECT(update_calls_ == 1, "run not called on invalid value");
  vl_destroy(v);
}

void test_trailing_tokens_after_action_are_rejected(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, true);
  reset_counters_();
  char *argv[] = {(char *)"prog", (char *)"--update", (char *)"universe"};
  EXPECT(v != NULL && vl_parse(v, 3, argv) == -1, "trailing verb rejected");
  const vl_error_t *err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNEXPECTED_ARGUMENT &&
             err->argv_index == 2,
         "unexpected argument at index 2");
  EXPECT(update_calls_ == 0, "run not called");
  EXPECT(!vl_reserved_fired(v), "not fired");
  vl_destroy(v);
}

void test_action_is_not_an_option_inside_a_verb(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, true);
  reset_counters_();
  char *argv[] = {(char *)"prog", (char *)"universe", (char *)"--update"};
  EXPECT(v != NULL && vl_parse(v, 3, argv) == -1, "scope leak rejected");
  const vl_error_t *err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNKNOWN_OPTION, "unknown option");
  EXPECT(update_calls_ == 0, "run not called");
  vl_destroy(v);
}

void test_action_is_not_an_option_without_verbs(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, false);
  reset_counters_();
  char *argv[] = {(char *)"prog", (char *)"--verbose", (char *)"--update"};
  EXPECT(v != NULL && vl_parse(v, 3, argv) == -1, "second position rejected");
  const vl_error_t *err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNKNOWN_OPTION, "unknown option");
  vl_destroy(v);
}

void test_reserved_and_verbs_still_work_with_actions(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, true);
  reset_counters_();

  char *ver[] = {(char *)"prog", (char *)"--version"};
  EXPECT(v != NULL && vl_parse(v, 2, ver) == 0, "--version still reserved");
  EXPECT(vl_reserved_fired(v) && vl_action_fired(v) == NULL,
         "reserved without action");

  char *verb[] = {(char *)"prog", (char *)"universe", (char *)"--count=2"};
  EXPECT(vl_parse(v, 3, verb) == 0, "verb parses");
  EXPECT(strcmp(vl_verb_get(v), "universe") == 0, "verb selected");
  EXPECT(vl_action_fired(v) == NULL, "no action on verb path");

  char *missing[] = {(char *)"prog", (char *)"--verbose"};
  EXPECT(vl_parse(v, 2, missing) == -1, "global at argv[1] still needs verb");
  EXPECT(update_calls_ == 0, "run never called");
  vl_destroy(v);
}

void test_action_find_token_shapes(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_,
                                                   &telemetry_toggle_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, false);
  EXPECT(v != NULL, "parser");
  EXPECT(vl_action_find_(NULL, "--update") == NULL, "null parser");
  EXPECT(vl_action_find_(v, NULL) == NULL, "null token");
  EXPECT(vl_action_find_(v, "update") == NULL, "no dash");
  EXPECT(vl_action_find_(v, "-") == NULL, "lone dash");
  EXPECT(vl_action_find_(v, "--") == NULL, "double dash");
  EXPECT(vl_action_find_(v, "--=x") == NULL, "empty name before =");
  EXPECT(vl_action_find_(v, "-x") == NULL, "unknown short");
  EXPECT(vl_action_find_(v, "-u") != NULL, "short");
  EXPECT(vl_action_find_(v, "--update") != NULL, "long");
  EXPECT(vl_action_find_(v, "--update=1") != NULL, "long inline");
  EXPECT(vl_action_find_(v, "--updated") == NULL, "prefix is not a match");
  EXPECT(vl_action_find_(v, "--upd8te") == NULL, "same length, other name");
  EXPECT(vl_action_find_(v, "--nope") == NULL, "unknown long");
  EXPECT(vl_action_find_(v, "--enable-telemetry") != NULL, "enable");
  EXPECT(vl_action_find_(v, "--disable-telemetry") != NULL, "disable");
  EXPECT(vl_action_find_(v, "--enable-") == NULL, "empty toggle ref");
  EXPECT(vl_action_find_(v, "--enable-nope") == NULL, "unknown toggle ref");
  vl_destroy(v);

  vl_executable_t plain = {.options = globals_};
  valve_t *p = vl_create(&plain);
  EXPECT(p != NULL && vl_action_find_(p, "--update") == NULL,
         "flag clear finds nothing");
  vl_destroy(p);
}

void test_action_fired_accessor_null(void) {
  EXPECT(vl_action_fired(NULL) == NULL, "null parser");
}

/* ---- help ------------------------------------------------------------- */

static int redirect_stdout_fd_(int fd) {
  fflush(stdout);
  int saved = dup(fileno(stdout));
  if (saved < 0)
    return -1;
  if (dup2(fd, fileno(stdout)) < 0) {
    close(saved);
    return -1;
  }
  return saved;
}

static void restore_stdout_(int saved) {
  fflush(stdout);
  if (saved >= 0) {
    dup2(saved, fileno(stdout));
    close(saved);
  }
}

static char *capture_help_(valve_t *v) {
  const char *path = ".cache/ceedling/actions_help_out.txt";
  FILE *out = fopen(path, "w+");
  if (!out)
    return NULL;
  int saved = redirect_stdout_fd_(fileno(out));
  if (saved < 0) {
    fclose(out);
    return NULL;
  }
  char *argv[] = {(char *)"prog", (char *)"--help"};
  int rc = vl_parse(v, 2, argv);
  restore_stdout_(saved);
  fclose(out);
  if (rc != 0)
    return NULL;
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;
  char *buf = calloc(1, 8192);
  if (buf)
    buf[fread(buf, 1, 8191, f)] = '\0';
  fclose(f);
  return buf;
}

void test_help_overview_lists_actions(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_,
                                                   &telemetry_toggle_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, true);
  EXPECT(v != NULL, "parser");
  char *out = capture_help_(v);
  EXPECT(out != NULL, "captured");
  EXPECT(out && strstr(out, "\nactions:\n"), "actions section");
  EXPECT(out && strstr(out, "replace this binary"), "action description");
  EXPECT(out && strstr(out, "reserved: --help  --version  --valve  --update"
                            "  --enable-telemetry | --disable-telemetry\n"),
         "reserved line lists actions");
  free(out);
  vl_destroy(v);
}

/* ---- lifecycle -------------------------------------------------------- */

void test_targets_clear_skips_actions_without_flag(void) {
  char *tag = strdup("keep");
  const vl_executable_action_t *const actions[] = {
      VL_ACT(.option = {.name = "update", .type = VL_OPT_TYPE_LONG,
                        .value = VL_OPTION_VALUE_STRING,
                        .target = VL_TARGET_STRING, .data = &tag},
             .run = run_update_),
      NULL, NULL};
  vl_executable_t settings = {.actions = actions};
  vl_targets_clear(&settings);
  EXPECT(tag != NULL, "flag clear leaves action targets alone");
  settings.behavior = VL_BEHAVIOR_ACCEPT_NO_VERB;
  settings.action_count = 2; /* explicit count with a null slot */
  vl_targets_clear(&settings);
  EXPECT(tag == NULL, "flag set clears and tolerates null slot");
}

void test_actions_clear_null_parser(void) {
  vl_actions_clear_(NULL);
  EXPECT(1, "null tolerated");
}

void test_create_action_allocation_failures(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_,
                                                   &telemetry_toggle_, NULL};
  vl_executable_t settings = {.behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
                              .options = globals_,
                              .verbs = verbs_,
                              .actions = actions,
                              .color = VAL_COLOR_NEVER};
  int created = 0;
  for (int n = 1; n <= 40; ++n) {
    vl_test_alloc_reset();
    vl_test_alloc_fail_after(n);
    valve_t *v = vl_create(&settings);
    vl_test_alloc_reset();
    if (v) {
      ++created;
      vl_destroy(v);
    }
  }
  EXPECT(created > 0, "create eventually succeeds past the fault window");
}

void test_parse_action_allocation_failure(void) {
  const vl_executable_action_t *const actions[] = {&update_bool_, NULL};
  valve_t *v = actions_parser_(actions, VL_ASSIGN_INLINE, true);
  EXPECT(v != NULL, "parser");
  reset_counters_();
  vl_test_alloc_fail_after(1);
  char *argv[] = {(char *)"prog", (char *)"--update"};
  EXPECT(vl_parse(v, 2, argv) == -1, "OOM while storing the value");
  vl_test_alloc_reset();
  EXPECT(update_calls_ == 0, "run not called on OOM");
  EXPECT(vl_action_fired(v) == NULL, "nothing fired on OOM");
  vl_destroy(v);
}
