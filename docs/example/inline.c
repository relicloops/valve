#include "valve.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

enum { MAX_CPU_COUNT = 64 };

typedef struct proxy_config {
  char *lane;
  int cpus;
} proxy_config_t;

typedef struct page_config {
  char *name;
  char *website;
  char *id;
  int limit;
  bool no_limit;
  bool enable_reload;
} page_config_t;

typedef struct app_config {
  proxy_config_t proxy;
  page_config_t page;
  vl_value_t tags;
  vl_value_t enable_log;
  bool verbose;
  vl_value_t meta;
  int64_t timeout_s;
  char *update_channel;
} app_config_t;

static app_config_t config;

/* ---- globals ---- */

static const vl_option_t opt_tags = {
    .name = "tags",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_ARRAY,
    .data = &config,
    .offset = offsetof(app_config_t, tags),
    .target = VL_TARGET_VALUE,
    .description = "Comma-separated tags.",
    .usage = "--tags=a,b,c",
};

static const vl_option_t opt_enable_log = {
    .name = "enable-log",
    .toggle_ref = "log",
    .type = VL_OPT_TYPE_TOGGLE,
    .value = VL_OPTION_VALUE_TOGGLE,
    .data = &config,
    .offset = offsetof(app_config_t, enable_log),
    .target = VL_TARGET_TOGGLE,
    .description = "Enable or disable logging.",
    .usage = "--enable-log / --disable-log",
};

static const vl_option_t opt_verbose = {
    .name = "verbose",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_BOOL,
    .data = &config,
    .offset = offsetof(app_config_t, verbose),
    .target = VL_TARGET_BOOL,
    .description = "Verbose output (-v / --version stay reserved).",
    .usage = "--verbose / --verbose=false",
};

static const vl_option_t opt_meta = {
    .name = "meta",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_KV,
    .data = &config,
    .offset = offsetof(app_config_t, meta),
    .target = VL_TARGET_VALUE,
    .description = "Key/value metadata bag.",
    .usage = "--meta='k:v|n:{x:1}'",
};

static const vl_option_t opt_timeout = {
    .name = "timeout",
    .short_name = 't',
    .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
    .value = VL_OPTION_VALUE_TIME,
    .data = &config,
    .offset = offsetof(app_config_t, timeout_s),
    .target = VL_TARGET_INT64,
    .has_int_min = true,
    .int_min = 0,
    .description = "Deadline as a duration (stored in seconds).",
    .usage = "--timeout=1h30m",
};

static const vl_option_t *const global_options[] = {
    &opt_tags,
    &opt_enable_log,
    &opt_verbose,
    &opt_meta,
    &opt_timeout,
    NULL,
};

/* ---- network verb (dotted options) ---- */

static const vl_option_t opt_proxy_lane = {
    .name = "proxy.lane",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_DOT_NOTATION,
    .data = &config.proxy,
    .offset = offsetof(proxy_config_t, lane),
    .target = VL_TARGET_STRING,
    .required = true,
    .description = "Proxy lane identifier.",
    .usage = "--proxy.lane=<lane>",
};

static const vl_option_t opt_proxy_cpus = {
    .name = "proxy.cpus",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_DOT_NOTATION,
    .data = &config.proxy,
    .offset = offsetof(proxy_config_t, cpus),
    .target = VL_TARGET_INT,
    .has_int_min = true,
    .has_int_max = true,
    .int_min = 1,
    .int_max = MAX_CPU_COUNT,
    .description = "Worker CPUs for the proxy.",
    .usage = "--proxy.cpus=<n>",
};

static const vl_option_t *const network_options[] = {
    &opt_proxy_lane,
    &opt_proxy_cpus,
    NULL,
};

static const vl_verb_t verb_network = {
    .name = "network",
    .options = network_options,
    .description = "Configure network proxy lanes.",
    .usage = "demo-inline network --proxy.lane=<lane> [--proxy.cpus=<n>]",
};

/* ---- page add sub-verb (requires / conflicts) ---- */

static const vl_option_t opt_page_name = {
    .name = "name",
    .short_name = 'n',
    .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
    .value = VL_OPTION_VALUE_STRING,
    .data = &config.page,
    .offset = offsetof(page_config_t, name),
    .target = VL_TARGET_STRING,
    .required = true,
    .description = "Page name.",
    .usage = "--name=<string>",
};

static const vl_option_t opt_page_website = {
    .name = "website",
    .short_name = 'w',
    .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
    .value = VL_OPTION_VALUE_STRING,
    .data = &config.page,
    .offset = offsetof(page_config_t, website),
    .target = VL_TARGET_STRING,
    .description = "Website host.",
    .usage = "--website=<host>",
};

static const vl_option_t opt_page_id = {
    .name = "id",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_STRING,
    .data = &config.page,
    .offset = offsetof(page_config_t, id),
    .target = VL_TARGET_STRING,
    .description = "Existing page id.",
    .usage = "--id=<string>",
};

static const vl_option_t opt_page_no_limit = {
    .name = "no-limit",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_BOOL,
    .data = &config.page,
    .offset = offsetof(page_config_t, no_limit),
    .target = VL_TARGET_BOOL,
    .description = "Disable the page size limit.",
};

static const vl_option_t *const opt_page_limit_conflicts[] = {
    &opt_page_no_limit,
    NULL,
};

static const vl_option_t opt_page_limit = {
    .name = "limit",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_INT,
    .data = &config.page,
    .offset = offsetof(page_config_t, limit),
    .target = VL_TARGET_INT,
    .conflicts = opt_page_limit_conflicts,
    .has_int_min = true,
    .int_min = 1,
    .description = "Page size limit.",
    .usage = "--limit=<n>",
};

static const vl_option_t *const opt_page_reload_requires[] = {
    &opt_page_id,
    NULL,
};

static const vl_option_t opt_page_enable_reload = {
    .name = "enable-reload",
    .type = VL_OPT_TYPE_LONG,
    .value = VL_OPTION_VALUE_BOOL,
    .data = &config.page,
    .offset = offsetof(page_config_t, enable_reload),
    .target = VL_TARGET_BOOL,
    .requires = opt_page_reload_requires,
    .description = "Reload after write (requires --id).",
};

static const vl_option_t *const page_add_options[] = {
    &opt_page_name,
    &opt_page_website,
    &opt_page_id,
    &opt_page_limit,
    &opt_page_no_limit,
    &opt_page_enable_reload,
    NULL,
};

static const vl_verb_t verb_page_add = {
    .name = "add",
    .options = page_add_options,
    .description = "Add a page.",
    .usage = "demo-inline page add --name=<string> [options]",
};

static const vl_verb_t *const page_subverbs[] = {
    &verb_page_add,
    NULL,
};

static const vl_verb_t verb_page = {
    .name = "page",
    .verbs = page_subverbs,
    .description = "Page management.",
    .usage = "demo-inline page <add> [options]",
};

static const vl_verb_t *const verbs[] = {
    &verb_network,
    &verb_page,
    NULL,
};

/* ---- executable actions (fire at argv[1], no verb) ---- */

static void run_update_(const valve_t *v) {
  (void)v;
  fprintf(stdout, "update: would replace demo-inline with the %s release\n",
          config.update_channel);
}

static void run_doctor_(const valve_t *v) {
  fprintf(stdout, "doctor: action %s fired, verb = %s\n", vl_action_fired(v),
          vl_verb_get(v) ? vl_verb_get(v) : "(none)");
}

static const vl_executable_action_t act_update = {
    .option =
        {
            .name = "update",
            .short_name = 'u',
            .type = VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT,
            .value = VL_OPTION_VALUE_STRING,
            .data = &config,
            .offset = offsetof(app_config_t, update_channel),
            .target = VL_TARGET_STRING,
            .description = "Replace this binary with a release channel.",
            .usage = "--update=<channel>",
        },
    .run = run_update_,
};

static const vl_executable_action_t act_doctor = {
    .option =
        {
            .name = "doctor",
            .type = VL_OPT_TYPE_LONG,
            .value = VL_OPTION_VALUE_BOOL,
            .description = "Check this installation and exit.",
        },
    .run = run_doctor_,
};

static const vl_executable_action_t *const actions[] = {
    &act_update,
    &act_doctor,
    NULL,
};

/* ---- dump helpers ---- */

static void print_value_(const vl_value_t *value) {
  if (!value)
    return;
  switch (value->kind) {
  case VL_VALUE_STRING:
    fputs(value->raw ? value->raw : "", stdout);
    break;
  case VL_VALUE_INT:
    fprintf(stdout, "%" PRId64, value->as.integer);
    break;
  case VL_VALUE_DOUBLE:
    fprintf(stdout, "%g", value->as.number);
    break;
  case VL_VALUE_BOOL:
    fputs(value->as.boolean ? "true" : "false", stdout);
    break;
  case VL_VALUE_KV:
    fputc('{', stdout);
    for (size_t i = 0; i < value->as.kv.count; ++i) {
      if (i)
        fputs(", ", stdout);
      fprintf(stdout, "%s=", value->as.kv.pairs[i].key);
      print_value_(&value->as.kv.pairs[i].value);
    }
    fputc('}', stdout);
    break;
  case VL_VALUE_ARRAY:
    fputc('[', stdout);
    for (size_t i = 0; i < value->as.array.count; ++i) {
      if (i)
        fputs(", ", stdout);
      print_value_(&value->as.array.items[i]);
    }
    fputc(']', stdout);
    break;
  case VL_VALUE_COMMAND:
    for (int i = 0; i < value->as.command.argc; ++i) {
      if (i)
        fputc(' ', stdout);
      fputs(value->as.command.argv[i], stdout);
    }
    break;
  }
}

static void print_parsed_(const valve_t *v) {
  const char *verb = vl_verb_get(v);
  const char *sub = vl_subverb_get(v);

  if (verb)
    fprintf(stdout, "verb = %s\n", verb);
  if (sub)
    fprintf(stdout, "subverb = %s\n", sub);

  if (config.proxy.lane)
    fprintf(stdout, "proxy.lane = %s\n", config.proxy.lane);
  if (config.proxy.cpus)
    fprintf(stdout, "proxy.cpus = %d\n", config.proxy.cpus);
  if (config.page.name)
    fprintf(stdout, "name = %s\n", config.page.name);
  if (config.page.website)
    fprintf(stdout, "website = %s\n", config.page.website);
  if (config.page.id)
    fprintf(stdout, "id = %s\n", config.page.id);
  if (vl_has(v, "limit"))
    fprintf(stdout, "limit = %d\n", config.page.limit);
  if (config.page.no_limit)
    fputs("no-limit = true\n", stdout);
  if (config.page.enable_reload)
    fputs("enable-reload = true\n", stdout);
  if (config.verbose)
    fputs("verbose = true\n", stdout);
  if (config.timeout_s)
    fprintf(stdout, "timeout = %" PRId64 "s\n", config.timeout_s);
  if (config.tags.raw || config.tags.kind == VL_VALUE_ARRAY) {
    fputs("tags = ", stdout);
    print_value_(&config.tags);
    fputc('\n', stdout);
  }
  if (config.enable_log.raw || config.enable_log.kind == VL_VALUE_BOOL) {
    fputs("enable-log = ", stdout);
    print_value_(&config.enable_log);
    fputc('\n', stdout);
  }
  if (config.meta.raw || config.meta.kind == VL_VALUE_KV) {
    fputs("meta = ", stdout);
    print_value_(&config.meta);
    fputc('\n', stdout);
  }
}

int main(int argc, char **argv) {
  const vl_executable_t settings = {
      .program_name = "demo-inline",
      .program_version = "0.1.0",
      .description = "Complete Valve example (inline assign).",
      .usage = "demo-inline <verb> [options]",
      .assign = VL_ASSIGN_INLINE,
      .color = VAL_COLOR_AUTO,
      .behavior = VL_BEHAVIOR_ACCEPT_NO_VERB,
      .options = global_options,
      .verbs = verbs,
      .actions = actions,
  };

  valve_t *v = vl_create(&settings);
  if (!v) {
    fputs("vl_create failed\n", stderr);
    return 1;
  }

  int rc = 0;
  if (vl_parse(v, argc, argv) != 0) {
    vl_errors_print(v, stderr);
    rc = 2;
  } else if (!vl_reserved_fired(v)) {
    print_parsed_(v);
  }

  vl_destroy(v);
  vl_targets_clear(&settings);
  return rc;
}
