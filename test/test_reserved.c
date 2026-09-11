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


void setUp(void) {}
void tearDown(void) {}

static int help_calls_ = 0;
static int version_calls_ = 0;
static int valve_calls_ = 0;
static const char *help_active_name_ = NULL;

static void count_help_(const valve_t *v) {
  ++help_calls_;
  help_active_name_ = vl_verb_get(v);
}

static void count_version_(const valve_t *v) {
  (void)v;
  ++version_calls_;
}

static void count_valve_(const valve_t *v) {
  (void)v;
  ++valve_calls_;
}

static void reset_callback_counters_(void) {
  help_calls_ = 0;
  version_calls_ = 0;
  valve_calls_ = 0;
  help_active_name_ = NULL;
}

static valve_t *reserved_parser_(void) {
  static const vl_option_t *const opts[] = {NULL};
  vl_executable_t settings = {
      .options = opts,
      .on_help = count_help_,
      .on_version = count_version_,
      .on_valve = count_valve_,
  };
  return vl_create(&settings);
}

static valve_t *reserved_command_parser_(void) {
  const vl_option_t *const network_opts[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_option_t *const config_opts[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const cmds[] = {
      VL_CMD(.name = "network", .options = network_opts),
      VL_CMD(.name = "config", .options = config_opts),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = cmds,
      .on_help = count_help_,
      .on_version = count_version_,
      .on_valve = count_valve_,
  };
  return vl_create(&settings);
}

static valve_t *reserved_subverb_parser_(void) {
  const vl_verb_t *const agent_subs[] = {
      VL_CMD(.name = "list", .description = "list agents"),
      VL_CMD(.name = "call", .description = "call an agent"),
      NULL,
  };
  const vl_verb_t *const page_subs[] = {
      VL_CMD(.name = "list", .description = "list pages"),
      NULL,
  };
  const vl_option_t *const network_opts[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_INT),
      NULL,
  };
  const vl_verb_t *const cmds[] = {
      VL_CMD(.name = "agent", .description = "agent ops", .verbs = agent_subs),
      VL_CMD(.name = "page", .description = "page ops", .verbs = page_subs),
      VL_CMD(.name = "network", .description = "net", .options = network_opts),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = cmds,
      .on_help = count_help_,
      .on_version = count_version_,
      .on_valve = count_valve_,
  };
  return vl_create(&settings);
}

void test_reserved_help_dotted_subverb(void) {
  valve_t *v = reserved_subverb_parser_();
  char *a1[] = {(char *)"valve", (char *)"--help=agent.list"};
  char *a2[] = {(char *)"valve", (char *)"--help=page.list"};

  reset_callback_counters_();
  EXPECT(v != NULL, "dotted-subverb parser created");
  EXPECT(vl_parse(v, 2, a1) == 0, "--help=agent.list parses");
  EXPECT(help_calls_ == 1, "--help=agent.list fires on_help");
  EXPECT(vl_help_target(v) && strcmp(vl_help_target(v), "agent.list") == 0,
         "help target stored as agent.list");
  EXPECT(vl_parse(v, 2, a2) == 0, "--help=page.list parses");
  EXPECT(vl_help_target(v) && strcmp(vl_help_target(v), "page.list") == 0,
         "help target stored as page.list (disambiguated)");
  vl_destroy(v);
}

void test_reserved_help_dotted_option_and_group(void) {
  valve_t *v = reserved_subverb_parser_();
  char *opt[] = {(char *)"valve", (char *)"--help=proxy.lane"};
  char *grp[] = {(char *)"valve", (char *)"--help=proxy"};
  char *nested[] = {(char *)"valve", (char *)"--help=agent.call"};

  reset_callback_counters_();
  EXPECT(v != NULL, "dotted-option parser created");
  EXPECT(vl_parse(v, 2, opt) == 0, "--help=proxy.lane parses");
  EXPECT(vl_parse(v, 2, grp) == 0, "--help=proxy (group) parses");
  EXPECT(vl_parse(v, 2, nested) == 0, "--help=agent.call parses");
  EXPECT(help_calls_ == 3, "all three dotted targets fire on_help");
  vl_destroy(v);
}

void test_reserved_help_ambiguous_list(void) {
  valve_t *v = reserved_subverb_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help=list"};
  const vl_error_t *err = NULL;

  reset_callback_counters_();
  EXPECT(v != NULL, "ambiguous-help parser created");
  EXPECT(vl_parse(v, 2, argv) == -1, "--help=list (ambiguous) fails to parse");
  EXPECT(help_calls_ == 0, "ambiguous --help=list does not fire on_help");
  EXPECT(vl_error_count(v) >= 1, "ambiguous --help=list records an error");
  err = vl_error_at(v, 0);
  EXPECT(err && err->message && strstr(err->message, "did you mean") != NULL,
         "ambiguous help error suggests the dotted forms");
  vl_destroy(v);
}

void test_reserved_long_help(void) {
  valve_t *v = reserved_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help"};

  reset_callback_counters_();
  EXPECT(v != NULL, "reserved parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "--help parses without error");
  EXPECT(help_calls_ == 1, "--help triggers on_help");
  EXPECT(help_active_name_ == NULL, "no active verb for top-level --help");
  vl_destroy(v);
}

void test_reserved_short_help(void) {
  valve_t *v = reserved_parser_();
  char *argv[] = {(char *)"valve", (char *)"-h"};

  reset_callback_counters_();
  EXPECT(v != NULL, "reserved parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "-h parses without error");
  EXPECT(help_calls_ == 1, "-h triggers on_help");
  vl_destroy(v);
}

void test_reserved_bare_help(void) {
  valve_t *v = reserved_command_parser_();
  char *argv[] = {(char *)"valve", (char *)"help"};

  reset_callback_counters_();
  EXPECT(v != NULL, "reserved verb parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "bare 'help' parses without error");
  EXPECT(help_calls_ == 1, "bare 'help' triggers on_help");
  vl_destroy(v);
}

void test_reserved_question_mark_variants(void) {
  valve_t *v = reserved_command_parser_();
  char *argv_long[] = {(char *)"valve", (char *)"--?"};
  char *argv_short[] = {(char *)"valve", (char *)"-?"};
  char *argv_bare[] = {(char *)"valve", (char *)"?"};

  reset_callback_counters_();
  EXPECT(v != NULL, "question-mark parser created");
  EXPECT(vl_parse(v, 2, argv_long) == 0, "--? parses");
  EXPECT(vl_parse(v, 2, argv_short) == 0, "-? parses");
  EXPECT(vl_parse(v, 2, argv_bare) == 0, "bare ? parses");
  EXPECT(help_calls_ == 3, "all three ? variants trigger on_help");
  vl_destroy(v);
}

void test_reserved_long_version(void) {
  valve_t *v = reserved_parser_();
  char *argv[] = {(char *)"valve", (char *)"--version"};

  reset_callback_counters_();
  EXPECT(v != NULL, "reserved parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "--version parses");
  EXPECT(version_calls_ == 1, "--version triggers on_version");
  vl_destroy(v);
}

void test_reserved_short_version(void) {
  valve_t *v = reserved_parser_();
  char *argv[] = {(char *)"valve", (char *)"-v"};

  reset_callback_counters_();
  EXPECT(v != NULL, "reserved parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "-v parses");
  EXPECT(version_calls_ == 1, "-v triggers on_version");
  vl_destroy(v);
}

void test_reserved_bare_version(void) {
  valve_t *v = reserved_command_parser_();
  char *argv[] = {(char *)"valve", (char *)"version"};

  reset_callback_counters_();
  EXPECT(v != NULL, "reserved verb parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "bare 'version' parses");
  EXPECT(version_calls_ == 1, "bare 'version' triggers on_version");
  vl_destroy(v);
}

void test_reserved_valve_variants(void) {
  valve_t *v = reserved_command_parser_();
  char *argv_long[] = {(char *)"valve", (char *)"--valve"};
  char *argv_dash[] = {(char *)"valve", (char *)"-valve"};
  char *argv_bare[] = {(char *)"valve", (char *)"valve"};

  reset_callback_counters_();
  EXPECT(v != NULL, "valve-variants parser created");
  EXPECT(vl_parse(v, 2, argv_long) == 0, "--valve parses");
  EXPECT(vl_parse(v, 2, argv_dash) == 0, "-valve parses");
  EXPECT(vl_parse(v, 2, argv_bare) == 0, "bare 'valve' parses");
  EXPECT(valve_calls_ == 3, "all three valve variants trigger on_valve");
  vl_destroy(v);
}

void test_reserved_help_context_aware(void) {
  valve_t *v = reserved_command_parser_();
  char *argv[] = {(char *)"valve", (char *)"network", (char *)"--help"};

  reset_callback_counters_();
  EXPECT(v != NULL, "context-aware parser created");
  EXPECT(vl_parse(v, 3, argv) == 0, "valve network --help parses");
  EXPECT(help_calls_ == 1, "on_help fired once");
  EXPECT(help_active_name_ && strcmp(help_active_name_, "network") == 0,
         "on_help received active verb = network");
  vl_destroy(v);
}

void test_reserved_help_default_when_null(void) {
  static const vl_option_t *const opts[] = {NULL};
  vl_executable_t settings = {.options = opts};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help"};

  EXPECT(v != NULL, "parser with NULL callbacks");
  EXPECT(vl_parse(v, 2, argv) == 0, "--help with NULL callback returns 0");
  vl_destroy(v);
}

void test_behavior_allow_override_reserved_option(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "help", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  vl_executable_t settings = {
      .options = options,
      .behavior = VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "option 'help' accepted with ALLOW_OVERRIDE_RESERVED");
  vl_destroy(v);
}

void test_behavior_allow_override_reserved_command(void) {
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "version"),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = verbs,
      .behavior = VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "verb 'version' accepted with ALLOW_OVERRIDE_RESERVED");
  vl_destroy(v);
}

void test_behavior_allow_override_user_wins_at_parse(void) {
  const vl_option_t *const options[] = {
      VL_OPT(.name = "help", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  vl_executable_t settings = {
      .options = options,
      .behavior = VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED,
      .on_help = count_help_,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help=foo"};
  const vl_value_t *value = NULL;

  reset_callback_counters_();
  EXPECT(v != NULL, "user-override parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "--help=foo parses as user option");
  EXPECT(help_calls_ == 0, "on_help NOT fired when user override exists");
  value = vl_get(v, "help");
  EXPECT(value && value->kind == VL_VALUE_STRING &&
             strcmp(value->raw, "foo") == 0,
         "user 'help' option captured 'foo'");
  vl_destroy(v);
}

void test_behavior_allow_override_falls_through_when_no_user_def(void) {
  static const vl_option_t *const opts[] = {NULL};
  vl_executable_t settings = {
      .options = opts,
      .behavior = VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED,
      .on_version = count_version_,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--version"};

  reset_callback_counters_();
  EXPECT(v != NULL, "allow-override-no-user-def parser");
  EXPECT(vl_parse(v, 2, argv) == 0, "--version parses");
  EXPECT(version_calls_ == 1, "on_version fires when no user override defined");
  vl_destroy(v);
}

