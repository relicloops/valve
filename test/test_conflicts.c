#include "test_support.h"

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

static const vl_option_t conflict_beta_ = {
    .name = "beta",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_BOOL,
};

static const vl_option_t *const conflict_alpha_conflicts_[] = {
    &conflict_beta_,
    NULL,
};

static const vl_option_t conflict_alpha_ = {
    .name = "alpha",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_BOOL,
    .conflicts = conflict_alpha_conflicts_,
};

static const vl_option_t *const conflict_globals_[] = {
    &conflict_alpha_,
    &conflict_beta_,
    NULL,
};

static valve_t *conflict_parser_(void) {
  vl_executable_t settings = {
      .options = conflict_globals_,
      .program_name = "valve-test",
      .description = "conflict coverage",
      .color = VAL_COLOR_NEVER,
  };
  return vl_create(&settings);
}

void test_conflict_both_present_fails(void) {
  valve_t *v = conflict_parser_();
  char *argv[] = {(char *)"valve", (char *)"--alpha", (char *)"--beta"};
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "conflict parser created");
  EXPECT(vl_parse(v, 3, argv) == -1, "both conflict members fail");
  EXPECT(vl_error_count(v) >= 1, "conflict records an error");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_CONFLICTING_OPTION,
         "conflict error code");
  EXPECT(err && err->message && strstr(err->message, "conflicts with") != NULL,
         "conflict message names other option");
  vl_destroy(v);
}

void test_conflict_order_independent(void) {
  valve_t *v = conflict_parser_();
  char *argv[] = {(char *)"valve", (char *)"--beta", (char *)"--alpha"};
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "conflict parser created");
  EXPECT(vl_parse(v, 3, argv) == -1, "reverse order also fails");
  err = vl_error_at(v, 0);
  EXPECT(err && err->code == VL_ERROR_CONFLICTING_OPTION,
         "reverse order conflict code");
  EXPECT(err && strcmp(err->key, "alpha") == 0,
         "later argv member is the error key");
  vl_destroy(v);
}

void test_conflict_single_member_ok(void) {
  valve_t *v = conflict_parser_();
  char *a1[] = {(char *)"valve", (char *)"--alpha"};
  char *a2[] = {(char *)"valve", (char *)"--beta"};

  EXPECT(v != NULL, "conflict parser created");
  EXPECT(vl_parse(v, 2, a1) == 0, "alpha alone ok");
  vl_destroy(v);

  v = conflict_parser_();
  EXPECT(vl_parse(v, 2, a2) == 0, "beta alone ok");
  vl_destroy(v);
}

void test_conflict_false_bool_still_conflicts(void) {
  valve_t *v = conflict_parser_();
  char *argv[] = {(char *)"valve", (char *)"--alpha=false",
                  (char *)"--beta=false"};

  EXPECT(v != NULL, "conflict parser created");
  EXPECT(vl_parse(v, 3, argv) == -1, "explicit false still conflicts");
  EXPECT(vl_error_at(v, 0) &&
             vl_error_at(v, 0)->code == VL_ERROR_CONFLICTING_OPTION,
         "false bool conflict code");
  vl_destroy(v);
}

void test_conflict_help_annotations(void) {
  valve_t *v = conflict_parser_();
  char *argv[] = {(char *)"valve", (char *)"--help"};
  const char *path = ".cache/ceedling/conflict_help_out.txt";
  FILE *out = fopen(path, "w+");
  int saved = -1;
  char *buf = NULL;

  EXPECT(v != NULL && out != NULL, "conflict help setup");
  saved = redirect_stdout_fd_(fileno(out));
  EXPECT(saved >= 0, "stdout redirected");
  EXPECT(vl_parse(v, 2, argv) == 0, "default --help parses");
  restore_stdout_(saved);
  fclose(out);

  buf = slurp_path_(path);
  EXPECT(buf && strstr(buf, "conflicts:") != NULL,
         "help shows conflicts annotation");
  EXPECT(buf && strstr(buf, "--beta") != NULL, "help lists beta");
  EXPECT(buf && strstr(buf, "--alpha") != NULL, "help lists alpha");
  free(buf);
  vl_destroy(v);
}

void test_conflict_invalid_self(void) {
  static vl_option_t loop_opt;
  static const vl_option_t *self_table[2];

  loop_opt = (vl_option_t){
      .name = "loop",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  self_table[0] = &loop_opt;
  self_table[1] = NULL;
  loop_opt.conflicts = self_table;

  const vl_option_t *const options[] = {&loop_opt, NULL};
  vl_executable_t settings = {.options = options};
  EXPECT(vl_create(&settings) == NULL, "self-conflict schema rejected");
}

void test_conflict_invalid_null_table_with_count(void) {
  static const vl_option_t bad = {
      .name = "bad",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
      .conflicts = NULL,
      .conflict_count = 1,
  };
  const vl_option_t *const options[] = {&bad, NULL};
  vl_executable_t settings = {.options = options};
  EXPECT(vl_create(&settings) == NULL,
         "conflict_count without table rejected");
}

void test_conflict_invalid_duplicate_target(void) {
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
      .conflicts = dups,
  };
  const vl_option_t *const options[] = {&source, &other, NULL};
  vl_executable_t settings = {.options = options};
  EXPECT(vl_create(&settings) == NULL, "duplicate conflict target rejected");
}

void test_conflict_reciprocal_ok(void) {
  static vl_option_t recip_a;
  static vl_option_t recip_b;
  static const vl_option_t *recip_a_c[2];
  static const vl_option_t *recip_b_c[2];

  recip_a = (vl_option_t){
      .name = "a",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  recip_b = (vl_option_t){
      .name = "b",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  recip_a_c[0] = &recip_b;
  recip_a_c[1] = NULL;
  recip_b_c[0] = &recip_a;
  recip_b_c[1] = NULL;
  recip_a.conflicts = recip_a_c;
  recip_b.conflicts = recip_b_c;

  const vl_option_t *const options[] = {&recip_a, &recip_b, NULL};
  vl_executable_t settings = {
      .options = options,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--a", (char *)"--b"};

  EXPECT(v != NULL, "reciprocal conflict schema accepted");
  EXPECT(vl_parse(v, 3, argv) == -1, "reciprocal still conflicts at parse");
  vl_destroy(v);
}

void test_conflict_capacity_growth(void) {
  static vl_option_t opts[10];
  static const vl_option_t *all[11];
  static const vl_option_t *c0_conflicts[10];
  static const char *names[] = {"c0", "c1", "c2", "c3", "c4",
                                "c5", "c6", "c7", "c8", "c9"};

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
    c0_conflicts[i] = &opts[i + 1];
  c0_conflicts[9] = NULL;
  opts[0].conflicts = c0_conflicts;

  vl_executable_t settings = {
      .options = all,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  EXPECT(v != NULL, "many-conflict schema created (capacity growth)");
  vl_destroy(v);
}

void test_conflict_on_verb_options(void) {
  static const vl_option_t local_b = {
      .name = "quiet",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  static const vl_option_t *const local_a_c[] = {&local_b, NULL};
  static const vl_option_t local_a = {
      .name = "loud",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
      .conflicts = local_a_c,
  };
  const vl_option_t *const local_opts[] = {&local_a, &local_b, NULL};
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "run", .options = local_opts),
      NULL,
  };
  vl_executable_t settings = {
      .verbs = verbs,
      .color = VAL_COLOR_NEVER,
  };
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"run", (char *)"--loud",
                  (char *)"--quiet"};

  EXPECT(v != NULL, "verb conflict parser created");
  EXPECT(vl_parse(v, 4, argv) == -1, "verb-local conflict fails");
  EXPECT(vl_error_at(v, 0) &&
             vl_error_at(v, 0)->code == VL_ERROR_CONFLICTING_OPTION,
         "verb-local conflict code");
  vl_destroy(v);
}

void test_conflict_short_option_label(void) {
  static const vl_option_t short_b = {
      .name = "brief",
      .short_name = 'b',
      .type = VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL,
  };
  static const vl_option_t *const short_a_c[] = {&short_b, NULL};
  static const vl_option_t short_a = {
      .name = "verbose",
      .short_name = 'V',
      .type = VL_OPT_TYPE_SHORT,
      .value = VL_OPTION_VALUE_BOOL,
      .conflicts = short_a_c,
  };
  static const vl_option_t *const opts[] = {&short_a, &short_b, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"-V", (char *)"-b"};
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "short conflict parser");
  EXPECT(vl_parse(v, 3, argv) == -1, "short conflict fails");
  err = vl_error_at(v, 0);
  EXPECT(err && err->message && strstr(err->message, "-V") != NULL,
         "conflict label uses short form");
  vl_destroy(v);
}

void test_conflict_counted_table(void) {
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
      .conflicts = table,
      .conflict_count = 1,
  };
  static const vl_option_t *const opts[] = {&source, &other, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--source", (char *)"--other"};

  EXPECT(v != NULL, "explicit conflict_count schema ok");
  EXPECT(vl_parse(v, 3, argv) == -1, "counted conflict fails parse");
  vl_destroy(v);
}

void test_conflict_toggle_label(void) {
  static const vl_option_t plain = {
      .name = "plain",
      .type = VL_OPT_TYPE_LONG,
      .value = VL_OPTION_VALUE_BOOL,
  };
  static const vl_option_t *const toggle_c[] = {&plain, NULL};
  static const vl_option_t toggle = {
      .name = "enable-feature",
      .toggle_ref = "feature",
      .type = VL_OPT_TYPE_TOGGLE,
      .value = VL_OPTION_VALUE_TOGGLE,
      .conflicts = toggle_c,
  };
  static const vl_option_t *const opts[] = {&toggle, &plain, NULL};
  vl_executable_t settings = {.options = opts, .color = VAL_COLOR_NEVER};
  valve_t *v = vl_create(&settings);
  char *argv[] = {(char *)"valve", (char *)"--enable-feature",
                  (char *)"--plain"};
  const vl_error_t *err = NULL;

  EXPECT(v != NULL, "toggle conflict parser");
  EXPECT(vl_parse(v, 3, argv) == -1, "toggle conflict fails");
  err = vl_error_at(v, 0);
  EXPECT(err && err->message &&
             strstr(err->message, "--enable-feature") != NULL,
         "toggle conflict labels enable/disable form");
  vl_destroy(v);
}
