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



#include <unistd.h>

void setUp(void) {}
void tearDown(void) { vl_color_support_reset(); }


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

static char *slurp_path_(const char *path) {
  FILE *f = fopen(path, "rb");
  char *buf = NULL;
  long sz;

  if (!f)
    return NULL;
  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return NULL;
  }
  sz = ftell(f);
  if (sz < 0) {
    fclose(f);
    return NULL;
  }
  rewind(f);
  buf = malloc((size_t)sz + 1);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
    free(buf);
    fclose(f);
    return NULL;
  }
  buf[sz] = '\0';
  fclose(f);
  return buf;
}

static char *capture_parse_(valve_t *v, int argc, char **argv) {
  const char *path = ".cache/ceedling/help_print_out.txt";
  FILE *out = fopen(path, "w+");
  int saved;
  char *buf;

  if (!out)
    return NULL;
  saved = redirect_stdout_fd_(fileno(out));
  if (saved < 0) {
    fclose(out);
    return NULL;
  }
  if (vl_parse(v, argc, argv) != 0) {
    restore_stdout_(saved);
    fclose(out);
    return NULL;
  }
  restore_stdout_(saved);
  fclose(out);
  buf = slurp_path_(path);
  return buf;
}

static valve_t *rich_parser_(void) {
  const vl_option_t *const agent_call_opts[] = {
      VL_OPT(.name = "request", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING, .usage = "JSON body",
             .description = "request payload", .required = true),
      NULL,
  };
  const vl_verb_t *const agent_subs[] = {
      VL_CMD(.name = "list", .description = "list agents",
             .usage = "valve agent list"),
      VL_CMD(.name = "call", .description = "call an agent",
             .options = agent_call_opts, .usage = "valve agent call"),
      NULL,
  };
  const vl_option_t *const net_opts[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING,
             .description = "lane name"),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_INT,
             .description = "cpu count"),
      VL_OPT(.name = "ratio", .short_name = 'r',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_DOUBLE),
      VL_OPT(.name = "enable-tls", .toggle_ref = "tls",
             .type = VL_OPT_TYPE_TOGGLE, .value = VL_OPTION_VALUE_TOGGLE),
      VL_OPT(.name = "tags", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_KV),
      VL_OPT(.name = "wait", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_TIME),
      VL_OPT(.name = "items", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_ARRAY),
      VL_OPT(.name = "score", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_NUMBER),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .description = "agent ops", .verbs = agent_subs,
             .usage = "valve agent <sub>"),
      VL_CMD(.name = "network", .description = "network", .options = net_opts),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "verbose", .short_name = 'V',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL, .required = true,
             .description = "chatty"),
      NULL,
  };
  vl_executable_t settings = {
      .options = globals,
      .verbs = verbs,
      .program_name = "valve-help",
      .program_version = "9.9.9",
      .description = "help printer coverage",
      .usage = "valve-help [options] <verb>",
      .logo = "◆",
      .color = VAL_COLOR_NEVER,
  };
  return vl_create(&settings);
}

void test_help_print_overview(void) {
  valve_t *v = rich_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help"};
  char *buf;

  EXPECT(v != NULL, "rich help parser");
  buf = capture_parse_(v, 2, argv);
  EXPECT(buf != NULL, "overview help captured");
  EXPECT(buf && strstr(buf, "valve-help") != NULL, "shows program name");
  EXPECT(buf && strstr(buf, "9.9.9") != NULL, "shows version");
  EXPECT(buf && strstr(buf, "agent") != NULL, "lists verbs");
  EXPECT(buf && strstr(buf, "verbose") != NULL, "lists globals");
  EXPECT(buf && strstr(buf, "required") != NULL, "required annotation");
  free(buf);
  vl_destroy(v);
}

void test_help_print_verb_card(void) {
  valve_t *v = rich_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help=agent"};
  char *buf;

  EXPECT(v != NULL, "rich help parser");
  buf = capture_parse_(v, 2, argv);
  EXPECT(buf != NULL, "verb card captured");
  EXPECT(buf && strstr(buf, "verb") != NULL, "verb card marker");
  EXPECT(buf && strstr(buf, "list") != NULL, "lists subverbs");
  EXPECT(buf && strstr(buf, "call") != NULL, "lists call subverb");
  free(buf);
  vl_destroy(v);
}

void test_help_print_subverb_card(void) {
  valve_t *v = rich_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help=agent.call"};
  char *buf;

  EXPECT(v != NULL, "rich help parser");
  buf = capture_parse_(v, 2, argv);
  EXPECT(buf != NULL, "subverb card captured");
  EXPECT(buf && strstr(buf, "request") != NULL, "shows subverb options");
  EXPECT(buf && strstr(buf, "JSON body") != NULL, "shows option usage");
  free(buf);
  vl_destroy(v);
}

void test_help_print_option_card(void) {
  valve_t *v = rich_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help=proxy.lane"};
  char *buf;

  EXPECT(v != NULL, "rich help parser");
  buf = capture_parse_(v, 2, argv);
  EXPECT(buf != NULL, "option card captured");
  EXPECT(buf && strstr(buf, "option") != NULL, "option card marker");
  EXPECT(buf && strstr(buf, "proxy.lane") != NULL, "names option");
  free(buf);
  vl_destroy(v);
}

void test_help_print_group_card(void) {
  valve_t *v = rich_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help=proxy"};
  char *buf;

  EXPECT(v != NULL, "rich help parser");
  buf = capture_parse_(v, 2, argv);
  EXPECT(buf != NULL, "group card captured");
  EXPECT(buf && strstr(buf, "group") != NULL, "group card marker");
  EXPECT(buf && strstr(buf, "proxy.cpus") != NULL, "lists group members");
  free(buf);
  vl_destroy(v);
}

void test_help_print_value_labels(void) {
  valve_t *v = rich_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help=network"};
  char *buf;

  EXPECT(v != NULL, "rich help parser");
  buf = capture_parse_(v, 2, argv);
  EXPECT(buf != NULL, "network help captured");
  EXPECT(buf && strstr(buf, "--enable-tls") != NULL, "toggle usage");
  EXPECT(buf && strstr(buf, "<duration>") != NULL, "time value label");
  EXPECT(buf && strstr(buf, "<kv>") != NULL, "kv value label");
  EXPECT(buf && strstr(buf, "<array>") != NULL, "array value label");
  EXPECT(buf && strstr(buf, "<number>") != NULL, "number value label");
  EXPECT(buf && strstr(buf, "<double>") != NULL, "double value label");
  free(buf);
  vl_destroy(v);
}

void test_default_version_and_valve_printers(void) {
  const vl_option_t *const opts[] = {NULL};
  vl_executable_t settings = {
      .options = opts,
      .program_name = "valve-test",
      .program_version = "1.2.3-test",
      .color = VAL_COLOR_NEVER,
      /* NULL callbacks → defaults */
  };
  valve_t *v = vl_create(&settings);
  char *ver_argv[] = {(char *)"valve", (char *)"--version"};
  char *valve_argv[] = {(char *)"valve", (char *)"--valve"};
  char *buf;

  EXPECT(v != NULL, "default printer parser");
  buf = capture_parse_(v, 2, ver_argv);
  EXPECT(buf != NULL && strstr(buf, "1.2.3-test") != NULL,
         "default version printer");
  free(buf);

  buf = capture_parse_(v, 2, valve_argv);
  EXPECT(buf != NULL && strlen(buf) > 0, "default valve printer");
  free(buf);
  vl_destroy(v);
}

void test_help_assign_separate_form(void) {
  const vl_option_t *const opts[] = {
      VL_OPT(.name = "mode", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  vl_executable_t settings = {
      .options = opts,
      .assign = VL_ASSIGN_SEPARATE,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--help"};
  char *buf;

  EXPECT(v != NULL, "separate-assign parser");
  buf = capture_parse_(v, 2, argv);
  EXPECT(buf != NULL && strstr(buf, "--mode <string>") != NULL,
         "separate assign uses space in usage");
  free(buf);
  vl_destroy(v);
}
