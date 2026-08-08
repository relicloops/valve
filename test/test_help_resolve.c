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


void setUp(void) {}
void tearDown(void) {}

/* Exercises the public vl_help_resolve() over a hand-built vl_verb_t table,
 * mirroring how an app (architect) resolves --help=<target> against its own
 * program-lifetime schema. No parser is created; the resolver is pure. */
void test_help_resolve_cases(void) {
  const vl_option_t *const agent_call_opts[] = {
      VL_OPT(.name = "request", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const agent_subs[] = {
      VL_CMD(.name = "list", .description = "list agents"),
      VL_CMD(.name = "call", .description = "call an agent",
             .options = agent_call_opts),
      NULL,
  };
  const vl_verb_t *const page_subs[] = {
      VL_CMD(.name = "list", .description = "list pages"),
      NULL,
  };
  const vl_option_t *const net_opts[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      VL_OPT(.name = "proxy.cpus", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_INT),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .description = "agent ops", .verbs = agent_subs),
      VL_CMD(.name = "page", .description = "page ops", .verbs = page_subs),
      VL_CMD(.name = "network", .description = "network", .options = net_opts),
      NULL,
  };
  const vl_option_t *const globals[] = {
      VL_OPT(.name = "verbose", .short_name = 'V',
             .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };

  vl_help_resolution_t r = {0};

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "agent", &r) &&
             r.kind == VL_HELP_VERB && r.verb &&
             strcmp(r.verb->name, "agent") == 0,
         "resolve 'agent' -> VERB agent");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "agent.list", &r) &&
             r.kind == VL_HELP_SUBVERB && strcmp(r.verb->name, "agent") == 0 &&
             r.subverb && strcmp(r.subverb->name, "list") == 0,
         "resolve 'agent.list' -> SUBVERB agent.list");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "page.list", &r) &&
             r.kind == VL_HELP_SUBVERB && strcmp(r.verb->name, "page") == 0 &&
             r.subverb && strcmp(r.subverb->name, "list") == 0,
         "resolve 'page.list' -> SUBVERB page.list (disambiguated)");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "agent.call", &r) &&
             r.kind == VL_HELP_SUBVERB && strcmp(r.subverb->name, "call") == 0,
         "resolve 'agent.call' -> SUBVERB agent.call");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "agent.call.request", &r) &&
             r.kind == VL_HELP_OPTION && strcmp(r.verb->name, "agent") == 0 &&
             r.subverb && strcmp(r.subverb->name, "call") == 0 && r.option &&
             strcmp(r.option->name, "request") == 0,
         "resolve 'agent.call.request' -> OPTION request");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "proxy.lane", &r) &&
             r.kind == VL_HELP_OPTION && strcmp(r.verb->name, "network") == 0 &&
             r.option && strcmp(r.option->name, "proxy.lane") == 0,
         "resolve 'proxy.lane' -> OPTION (dotted option name)");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "proxy", &r) &&
             r.kind == VL_HELP_GROUP && r.group &&
             strcmp(r.group, "proxy") == 0,
         "resolve 'proxy' -> GROUP proxy");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "verbose", &r) &&
             r.kind == VL_HELP_OPTION && r.verb == NULL && r.option &&
             strcmp(r.option->name, "verbose") == 0,
         "resolve 'verbose' -> global OPTION");

  EXPECT(!vl_help_resolve(verbs, 0, globals, 0, "nope", &r) &&
             r.kind == VL_HELP_NONE,
         "resolve 'nope' -> NONE");

  EXPECT(vl_help_resolve(verbs, 0, globals, 0, "call", &r) &&
             r.kind == VL_HELP_SUBVERB && strcmp(r.verb->name, "agent") == 0 &&
             r.subverb && strcmp(r.subverb->name, "call") == 0,
         "bare 'call' resolves (unique sub-verb) -> SUBVERB agent.call");

  EXPECT(!vl_help_resolve(verbs, 0, globals, 0, "list", &r),
         "bare 'list' does not resolve (ambiguous sub-verb)");

  EXPECT(!vl_help_resolve(verbs, 0, globals, 0, "agent.request", &r),
         "'agent.request' (option not on verb) -> no match");

  EXPECT(!vl_help_resolve(verbs, 0, globals, 0, "", &r),
         "empty target -> no match");
}

/* A DOT_NOTATION option name already contains dots, so a verb-prefixed help
 * target splits into more segments than the walker has scopes. Everything
 * after the matched verb / sub-verb is rejoined into the option name, and a
 * trailing lone segment that names no option resolves as a group prefix. */
void test_help_resolve_dotted_option_suffix(void) {
  const vl_option_t *const call_opts[] = {
      VL_OPT(.name = "disposition.will", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_INT),
      VL_OPT(.name = "no-traits", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_BOOL),
      NULL,
  };
  const vl_verb_t *const agent_subs[] = {
      VL_CMD(.name = "call", .description = "call an agent",
             .options = call_opts),
      NULL,
  };
  const vl_option_t *const net_opts[] = {
      VL_OPT(.name = "proxy.lane", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "agent", .description = "agent ops", .verbs = agent_subs),
      VL_CMD(.name = "network", .description = "network", .options = net_opts),
      NULL,
  };

  vl_help_resolution_t r = {0};

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "agent.call.disposition.will", &r) &&
             r.kind == VL_HELP_OPTION && strcmp(r.verb->name, "agent") == 0 &&
             r.subverb && strcmp(r.subverb->name, "call") == 0 && r.option &&
             strcmp(r.option->name, "disposition.will") == 0,
         "resolve 'agent.call.disposition.will' -> OPTION disposition.will");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "network.proxy.lane", &r) &&
             r.kind == VL_HELP_OPTION && strcmp(r.verb->name, "network") == 0 &&
             r.subverb == NULL && r.option &&
             strcmp(r.option->name, "proxy.lane") == 0,
         "resolve 'network.proxy.lane' -> OPTION on the verb itself");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "network.proxy", &r) &&
             r.kind == VL_HELP_GROUP && r.group &&
             strcmp(r.group, "proxy") == 0 && r.verb &&
             strcmp(r.verb->name, "network") == 0,
         "resolve 'network.proxy' -> GROUP proxy scoped to the verb");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "agent.call.disposition", &r) &&
             r.kind == VL_HELP_GROUP && r.group &&
             strcmp(r.group, "disposition") == 0 && r.subverb &&
             strcmp(r.subverb->name, "call") == 0,
         "resolve 'agent.call.disposition' -> GROUP scoped to the sub-verb");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "agent.call.no-traits", &r) &&
             r.kind == VL_HELP_OPTION && r.option &&
             strcmp(r.option->name, "no-traits") == 0,
         "resolve 'agent.call.no-traits' -> OPTION (single-segment suffix)");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "proxy.lane", &r) &&
             r.kind == VL_HELP_OPTION && strcmp(r.verb->name, "network") == 0 &&
             r.option && strcmp(r.option->name, "proxy.lane") == 0,
         "bare 'proxy.lane' still resolves by exact name");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "agent.call", &r) &&
             r.kind == VL_HELP_SUBVERB && r.subverb &&
             strcmp(r.subverb->name, "call") == 0,
         "resolve 'agent.call' -> SUBVERB (sub-verb still wins at n == 2)");

  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "agent.call.disposition.missing",
                          &r),
         "'agent.call.disposition.missing' -> no match");

  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "network.missing", &r),
         "'network.missing' (no option, no group) -> no match");

  /* Past VL_PATH_MAX_SEGMENTS vl_path_split reports 0, so structural
   * resolution is skipped and only an exact option name could still match. */
  EXPECT(!vl_help_resolve(verbs, 0, NULL, 0, "agent.call.disposition.will.deep",
                          &r),
         "over-deep path skips structural resolution -> no match");
}

/* An explicit verb_count is taken verbatim, so a caller may hand over a verb
 * or sub-verb table with NULL slots. Walking past them must not fault. */
void test_help_resolve_null_verb_slots(void) {
  const vl_option_t *const solo_opts[] = {
      VL_OPT(.name = "g.a", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      NULL,
  };
  const vl_option_t *const only_opts[] = {
      VL_OPT(.name = "deep.leaf", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_DOT_NOTATION, .target = VL_TARGET_STRING),
      NULL,
  };
  /* Explicit counts, so the leading and trailing NULL slots stay visible to
   * the scanners instead of terminating the walk. */
  const vl_verb_t *const solo_subs[] = {
      NULL,
      VL_CMD(.name = "only", .description = "only sub", .options = only_opts),
      NULL,
  };
  const vl_verb_t *const sparse[] = {
      NULL,
      VL_CMD(.name = "solo", .description = "solo", .options = solo_opts,
             .verbs = solo_subs, .verb_count = 3),
      NULL,
  };

  vl_help_resolution_t r = {0};

  EXPECT(!vl_help_resolve(sparse, 3, NULL, 0, "nope", &r),
         "bare miss over NULL verb and sub-verb slots -> no match, no fault");

  EXPECT(vl_help_resolve(sparse, 3, NULL, 0, "g", &r) &&
             r.kind == VL_HELP_GROUP && r.group && strcmp(r.group, "g") == 0,
         "group_anywhere_ walks past NULL verb slots");

  EXPECT(vl_help_resolve(sparse, 3, NULL, 0, "g.a", &r) &&
             r.kind == VL_HELP_OPTION && r.option &&
             strcmp(r.option->name, "g.a") == 0,
         "option_anywhere_ walks past NULL verb slots");

  EXPECT(vl_help_resolve(sparse, 3, NULL, 0, "only", &r) &&
             r.kind == VL_HELP_SUBVERB && r.subverb &&
             strcmp(r.subverb->name, "only") == 0,
         "unique_subverb_ walks past NULL verb slots");

  EXPECT(vl_help_resolve(sparse, 3, NULL, 0, "deep.leaf", &r) &&
             r.kind == VL_HELP_OPTION && r.subverb &&
             strcmp(r.subverb->name, "only") == 0 && r.option &&
             strcmp(r.option->name, "deep.leaf") == 0,
         "option_anywhere_ walks past NULL sub-verb slots");

  EXPECT(vl_help_resolve(sparse, 3, NULL, 0, "deep", &r) &&
             r.kind == VL_HELP_GROUP && r.group && strcmp(r.group, "deep") == 0,
         "group_anywhere_ walks past NULL sub-verb slots");
}

/* Bare verb name must beat a same-named nested option (architect: generate
 * verb vs skinjo.call --generate). */
void test_help_resolve_verb_beats_option(void) {
  const vl_verb_t *const generate_subs[] = {
      VL_CMD(.name = "apikey", .description = "make a key"),
      NULL,
  };
  const vl_option_t *const skinjo_call_opts[] = {
      VL_OPT(.name = "generate", .type = VL_OPT_TYPE_LONG,
             .value = VL_OPTION_VALUE_STRING),
      NULL,
  };
  const vl_verb_t *const skinjo_subs[] = {
      VL_CMD(.name = "call", .description = "call skinjo",
             .options = skinjo_call_opts),
      NULL,
  };
  const vl_verb_t *const verbs[] = {
      VL_CMD(.name = "generate", .description = "secrets",
             .verbs = generate_subs),
      VL_CMD(.name = "skinjo", .description = "skinjo ops",
             .verbs = skinjo_subs),
      NULL,
  };

  vl_help_resolution_t r = {0};

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "generate", &r) &&
             r.kind == VL_HELP_VERB && r.verb &&
             strcmp(r.verb->name, "generate") == 0 && r.option == NULL,
         "bare 'generate' -> VERB (not skinjo.call --generate)");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "generate.apikey", &r) &&
             r.kind == VL_HELP_SUBVERB && strcmp(r.verb->name, "generate") == 0 &&
             r.subverb && strcmp(r.subverb->name, "apikey") == 0,
         "resolve 'generate.apikey' -> SUBVERB");

  EXPECT(vl_help_resolve(verbs, 0, NULL, 0, "skinjo.call.generate", &r) &&
             r.kind == VL_HELP_OPTION && strcmp(r.verb->name, "skinjo") == 0 &&
             r.subverb && strcmp(r.subverb->name, "call") == 0 && r.option &&
             strcmp(r.option->name, "generate") == 0,
         "resolve 'skinjo.call.generate' -> OPTION");
}

