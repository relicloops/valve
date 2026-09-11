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

static const vl_option_t req_id_ = {
    .name = "id",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_STRING,
};

static const vl_option_t *const req_enable_requires_[] = {
    &req_id_,
    NULL,
};

static const vl_option_t req_enable_ = {
    .name = "enable-reload",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_BOOL,
    .requires = req_enable_requires_,
};

static const vl_option_t *const req_globals_[] = {
    &req_enable_,
    &req_id_,
    NULL,
};

static valve_t *req_parser_(void) {
  vl_executable_t settings = {
      .options = req_globals_,
      .program_name = "valve-test",
      .description = "requirement coverage",
      .color = VAL_COLOR_NEVER,
  };
  return vl_create(&settings);
}

void test_requirement_missing_target_fails(void) {
  valve_t *v = req_parser_();
  char *argv[] = {(char *)"valve", (char *)"--enable-reload"};
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "requirement parser created");
  EXPECT(vl_parse(v, 2, argv) == -1, "source without target fails");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_UNSATISFIED_REQUIREMENT,
         "unsatisfied requirement code");
  EXPECT(err && err->message && strstr(err->message, "requires") != NULL,
         "requirement message");
  EXPECT(err && strcmp(err->key, "enable-reload") == 0,
         "error key is source option");
  vl_destroy(v);
}

void test_requirement_satisfied(void) {
  valve_t *v = req_parser_();
  char *argv[] = {(char *)"valve", (char *)"--enable-reload",
                  (char *)"--id=42"};

  EXPECT(v != NULL, "requirement parser created");
  EXPECT(vl_parse(v, 3, argv) == 0, "source+target succeeds");
  EXPECT(vl_has(v, "enable-reload") && vl_has(v, "id"),
         "both options present");
  vl_destroy(v);
}

void test_requirement_target_alone_ok(void) {
  valve_t *v = req_parser_();
  char *argv[] = {(char *)"valve", (char *)"--id=7"};

  EXPECT(v != NULL, "requirement parser created");
  EXPECT(vl_parse(v, 2, argv) == 0, "target alone does not require source");
  vl_destroy(v);
}

void test_requirement_false_source_still_requires(void) {
  valve_t *v = req_parser_();
  char *argv[] = {(char *)"valve", (char *)"--enable-reload=false"};

  EXPECT(v != NULL, "requirement parser created");
  EXPECT(vl_parse(v, 2, argv) == -1, "false source still activates requires");
  EXPECT(vl_error_at(v, 0) &&
             vl_error_at(v, 0)->code == VL_ERROR_UNSATISFIED_REQUIREMENT,
         "false source unsatisfied code");
  vl_destroy(v);
}

void test_requirement_help_annotations(void) {
  valve_t *v = req_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help"};
  const char *path = ".cache/ceedling/requirement_help_out.txt";
  FILE *out = fopen(path, "w+");
  int saved = -1;
  char *buf = NULL;

  EXPECT(v != NULL && out != NULL, "requirement help setup");
  saved = redirect_stdout_fd_(fileno(out));
  EXPECT(saved >= 0, "stdout redirected");
  EXPECT(vl_parse(v, 2, argv) == 0, "default --help parses");
  restore_stdout_(saved);
  fclose(out);

  buf = slurp_path_(path);
  EXPECT(buf && strstr(buf, "requires:") != NULL, "help shows requires");
  EXPECT(buf && strstr(buf, "required by:") != NULL, "help shows required by");
  EXPECT(buf && strstr(buf, "--id") != NULL, "help names id");
  free(buf);
  vl_destroy(v);
}

void test_requirement_invalid_self(void) {
  static vl_option_t loop_opt;
  static const vl_option_t *self_table[2];

  loop_opt = (vl_option_t){
      .name = "loop",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  self_table[0] = &loop_opt;
  self_table[1] = NULL;
  loop_opt.requires = self_table;

  const vl_option_t *const options[] = {&loop_opt, NULL};
  vl_executable_t settings = {.options = options};
  EXPECT(vl_create(&settings) == NULL, "self-require schema rejected");
}

void test_requirement_invalid_null_table_with_count(void) {
  static const vl_option_t bad = {
      .name = "bad",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
      .requires = NULL,
      .require_count = 1,
  };
  const vl_option_t *const options[] = {&bad, NULL};
  vl_executable_t settings = {.options = options};
  EXPECT(vl_create(&settings) == NULL,
         "require_count without table rejected");
}

void test_requirement_invalid_duplicate_target(void) {
  static const vl_option_t other = {
      .name = "other",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  static const vl_option_t *const dups[] = {&other, &other, NULL};
  static const vl_option_t source = {
      .name = "source",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
      .requires = dups,
  };
  const vl_option_t *const options[] = {&source, &other, NULL};
  vl_executable_t settings = {.options = options};
  EXPECT(vl_create(&settings) == NULL, "duplicate require target rejected");
}

void test_requirement_capacity_growth(void) {
  static vl_option_t opts[10];
  static const vl_option_t *all[11];
  static const vl_option_t *c0_requires[10];
  static const char *names[] = {"r0", "r1", "r2", "r3", "r4",
                                "r5", "r6", "r7", "r8", "r9"};

  for (int i = 0; i < 10; ++i) {
    opts[i] = (vl_option_t){
        .name = names[i],
        .type = VL_OPT_TYPE_LONG,
        .value = VL_OPTION_VALUE_BOOL,
    };
    all[i] = &opts[i];
  }
  all[10] = NULL;
  for (int i = 0; i < 9; ++i)
    c0_requires[i] = &opts[i + 1];
  c0_requires[9] = NULL;
  opts[0].requires = c0_requires;

  vl_executable_t settings = {
      .options = all,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "many-require schema created (capacity growth)");
  /* Trigger check path with source present and all targets missing. */
  char *argv[] = {(char *)"valve", (char *)"--r0"};
  EXPECT(vl_parse(v, 2, argv) == -1, "missing many requires fails");
  EXPECT(vl_error_count(v) >= 1, "records requirement errors");
  vl_destroy(v);
}

void test_requirement_on_verb_options(void) {
  static const vl_option_t token = {
      .name = "token",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_STRING,
  };
  static const vl_option_t *const pub_req[] = {&token, NULL};
  static const vl_option_t publish = {
      .name = "publish",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
      .requires = pub_req,
  };
  const vl_option_t *const local_opts[] = {&publish, &token, NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "ship", .options = local_opts),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *bad[] = {(char *)"valve", (char *)"ship", (char *)"--publish"};
  char *ok[] = {(char *)"valve", (char *)"ship", (char *)"--publish",
                (char *)"--token=abc"};

  EXPECT(v != NULL, "verb requirement parser");
  EXPECT(vl_parse(v, 3, bad) == -1, "verb-local missing require fails");
  vl_destroy(v);

  v = vl_create(&settings);
  EXPECT(vl_parse(v, 4, ok) == 0, "verb-local require satisfied");
  vl_destroy(v);
}

void test_requirement_cycle_a_requires_b_requires_a(void) {
  static vl_option_t a, b;
  static const vl_option_t *a_req[2];
  static const vl_option_t *b_req[2];

  a = (vl_option_t){
      .name = "a",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  b = (vl_option_t){
      .name = "b",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  a_req[0] = &b;
  a_req[1] = NULL;
  b_req[0] = &a;
  b_req[1] = NULL;
  a.requires = a_req;
  b.requires = b_req;

  const vl_option_t *const options[] = {&a, &b, NULL};
  vl_executable_t settings = {.options = options, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *both[] = {(char *)"valve", (char *)"--a", (char *)"--b"};
  char *only_a[] = {(char *)"valve", (char *)"--a"};

  EXPECT(v != NULL, "cyclic requires schema accepted");
  EXPECT(vl_parse(v, 3, both) == 0, "both sides of cycle ok");
  vl_destroy(v);

  v = vl_create(&settings);
  EXPECT(vl_parse(v, 2, only_a) == -1, "cycle source alone fails");
  vl_destroy(v);
}

void test_requirement_counted_table(void) {
  static const vl_option_t other = {
      .name = "other",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  static const vl_option_t *const table[] = {&other};
  static const vl_option_t source = {
      .name = "source",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
      .requires = table,
      .require_count = 1,
  };
  static const vl_option_t *const opts[] = {&source, &other, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--source", (char *)"--other"};

  EXPECT(v != NULL, "explicit require_count schema ok");
  EXPECT(vl_parse(v, 3, argv) == 0, "counted require satisfied");
  vl_destroy(v);
}
