#include "valve/color.h"

#include <langinfo.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*
 * Shared file-static color / term state. Kept in one TU because init,
 * enable, support detect, and term caps share this mutable configuration.
 */

static bool g_initialized_ = false;
static vl_color_mode_t g_mode_ = VAL_COLOR_AUTO;

static bool g_stdout_color_ = false;
static bool g_stderr_color_ = false;

static bool g_stdout_tty_ = false;
static bool g_stderr_tty_ = false;
static bool g_utf8_ = false;
static bool g_hyperlinks_ = false;
static unsigned g_columns_ = 80;
static unsigned g_rows_ = 24;

static vl_color_support_t g_support_ = VL_COLOR_SUPPORT_NONE;
static bool g_support_detected_ = false;
static bool g_support_forced_ = false;
static bool g_hyperlinks_detected_ = false;

static bool env_nonempty_(const char *name) {
  const char *v = getenv(name);
  return v && v[0] != '\0';
}

static const char *env_(const char *name) {
  const char *v = getenv(name);
  return v ? v : "";
}

static bool str_eq_(const char *a, const char *b) {
  return strcmp(a, b) == 0;
}

static bool str_contains_(const char *hay, const char *needle) {
  /* GCOVR_EXCL_BR_START — callers pass env_ "" / string literals, never NULL */
  return hay && needle && strstr(hay, needle) != NULL;
  /* GCOVR_EXCL_BR_STOP */
}

static void probe_tty_(void) {
  g_stdout_tty_ = isatty(STDOUT_FILENO) != 0;
  g_stderr_tty_ = isatty(STDERR_FILENO) != 0;
}

static void probe_utf8_(void) {
  const char *codeset = NULL;

  setlocale(LC_CTYPE, "");
  codeset = nl_langinfo(CODESET);
  if (codeset && (str_eq_(codeset, "UTF-8") || str_eq_(codeset, "utf8"))) { /* GCOVR_EXCL_BR_LINE: codeset NULL / non-utf8 secondary arm host-dependent */
    g_utf8_ = true;
    return;
  }

  const char *lang = env_("LC_ALL");
  if (!lang[0])
    lang = env_("LC_CTYPE");
  if (!lang[0])
    lang = env_("LANG");
  g_utf8_ = str_contains_(lang, "UTF-8") || str_contains_(lang, "utf8"); /* GCOVR_EXCL_BR_LINE: empty LANG after empty LC_* is host/locale dependent */
}

static void probe_winsize_(void) {
  struct winsize ws;

  g_columns_ = 80;
  g_rows_ = 24;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
    g_columns_ = ws.ws_col;
    if (ws.ws_row > 0) /* GCOVR_EXCL_BR_LINE: ioctl with row==0 not producible without mocking */
      g_rows_ = ws.ws_row;
    return;
  }

  const char *cols = getenv("COLUMNS");
  if (cols && cols[0]) {
    int n = atoi(cols);
    if (n > 0)
      g_columns_ = (unsigned)n;
  }
  const char *rows = getenv("LINES");
  if (rows && rows[0]) {
    int n = atoi(rows);
    if (n > 0)
      g_rows_ = (unsigned)n;
  }
}

/**
 * Detect color support. `stream_tty` is true when at least one of
 * stdout/stderr is a TTY (Valve gates fancy output on TTY; FORCE_COLOR
 * bypasses this via an early return).
 */
static vl_color_support_t detect_color_env_(vl_color_mode_t mode,
                                            bool stream_tty) {
  if (env_nonempty_("NO_COLOR"))
    return VL_COLOR_SUPPORT_NONE;

  const char *force = getenv("FORCE_COLOR");
  if (force && force[0] != '\0') {
    if (str_eq_(force, "0"))
      return VL_COLOR_SUPPORT_NONE;
    if (str_eq_(force, "1"))
      return VL_COLOR_SUPPORT_BASIC;
    if (str_eq_(force, "2"))
      return VL_COLOR_SUPPORT_256;
    if (str_eq_(force, "3"))
      return VL_COLOR_SUPPORT_TRUECOLOR;
    return VL_COLOR_SUPPORT_TRUECOLOR;
  }

  if (mode == VAL_COLOR_NEVER)
    return VL_COLOR_SUPPORT_NONE;

  if (!stream_tty)
    return VL_COLOR_SUPPORT_NONE;

  const char *colorterm = env_("COLORTERM");
  if (str_eq_(colorterm, "truecolor") || str_eq_(colorterm, "24bit")) /* GCOVR_EXCL_BR_LINE: 24bit arm needs TTY; CI often non-TTY */
    return VL_COLOR_SUPPORT_TRUECOLOR;

  const char *term_program = env_("TERM_PROGRAM");
  static const char *const truecolor_programs[] = {
      "iTerm.app", "Apple_Terminal", "Hyper", "vscode", NULL};
  for (size_t i = 0; truecolor_programs[i]; ++i) {
    if (str_eq_(term_program, truecolor_programs[i]))
      return VL_COLOR_SUPPORT_TRUECOLOR;
  }

  const char *term = env_("TERM");
  if (!term[0] || str_eq_(term, "dumb")) {
    /* ALWAYS + live TTY + non-dumb already excluded; dumb stays NONE. */
    if (mode == VAL_COLOR_ALWAYS && term[0] == '\0') /* GCOVR_EXCL_BR_LINE: ALWAYS+empty TERM needs TTY */
      return VL_COLOR_SUPPORT_BASIC;
    return VL_COLOR_SUPPORT_NONE;
  }

  /* GCOVR_EXCL_BR_START — TERM truecolor/24bit/direct needs TTY */
  if (str_contains_(term, "truecolor") || str_contains_(term, "24bit") ||
      str_contains_(term, "direct"))
    return VL_COLOR_SUPPORT_TRUECOLOR;
  /* GCOVR_EXCL_BR_STOP */

  if (str_contains_(term, "256color") || str_contains_(term, "256")) /* GCOVR_EXCL_BR_LINE: TERM heuristics need TTY */
    return VL_COLOR_SUPPORT_256;

  static const char *const color256_terms[] = {
      "xterm", "screen", "tmux", "rxvt", "linux", "ansi", "vt100", "vt220",
      NULL};
  for (size_t i = 0; color256_terms[i]; ++i) {
    if (str_contains_(term, color256_terms[i]))
      return VL_COLOR_SUPPORT_256;
  }

  /* TERM set to something unknown → BASIC. ALWAYS is already >= BASIC. */
  return VL_COLOR_SUPPORT_BASIC;
}

static bool detect_hyperlinks_(void) {
  const char *term_program = env_("TERM_PROGRAM");
  static const char *const hyperlink_programs[] = {"iTerm.app", "Hyper",
                                                   "vscode", NULL};
  for (size_t i = 0; hyperlink_programs[i]; ++i) {
    if (str_eq_(term_program, hyperlink_programs[i]))
      return true;
  }

  if (env_nonempty_("VTE_VERSION"))
    return true;
  if (env_nonempty_("KITTY_WINDOW_ID"))
    return true;
  if (env_nonempty_("WEZTERM_PANE"))
    return true;
  if (env_nonempty_("KONSOLE_VERSION"))
    return true;

  return vl_color_support_detect() >= VL_COLOR_SUPPORT_TRUECOLOR;
}

static void apply_enable_bits_(vl_color_mode_t mode) {
  bool forced_on = false;
  const char *force = getenv("FORCE_COLOR");
  if (!env_nonempty_("NO_COLOR") && force && force[0] != '\0' &&
      !str_eq_(force, "0"))
    forced_on = true;

  bool want = g_support_ >= VL_COLOR_SUPPORT_BASIC;
  g_stdout_color_ =
      want && (g_stdout_tty_ || forced_on || mode == VAL_COLOR_ALWAYS);
  g_stderr_color_ =
      want && (g_stderr_tty_ || forced_on || mode == VAL_COLOR_ALWAYS);
}

static void refresh_caps_(vl_color_mode_t mode) {
  probe_tty_();
  probe_utf8_();
  probe_winsize_();

  if (!g_support_forced_) {
    bool any_tty = g_stdout_tty_ || g_stderr_tty_;
    g_support_ = detect_color_env_(mode, any_tty);
    g_support_detected_ = true;
  }

  g_hyperlinks_ = detect_hyperlinks_();
  g_hyperlinks_detected_ = true;
  apply_enable_bits_(mode);
}

void vl_color_init(vl_color_mode_t mode) {
  g_mode_ = mode;
  g_initialized_ = true;
  g_support_detected_ = false;
  g_hyperlinks_detected_ = false;
  if (!g_support_forced_)
    g_support_ = VL_COLOR_SUPPORT_NONE;
  refresh_caps_(mode);
}

vl_color_support_t vl_color_support_detect(void) {
  if (g_support_forced_)
    return g_support_;
  if (!g_initialized_)
    vl_color_init(VAL_COLOR_AUTO);
  else if (!g_support_detected_)
    refresh_caps_(g_mode_);
  return g_support_;
}

bool vl_color_supports(void) {
  return vl_color_support_detect() >= VL_COLOR_SUPPORT_BASIC;
}

bool vl_color_supports_256(void) {
  return vl_color_support_detect() >= VL_COLOR_SUPPORT_256;
}

bool vl_color_supports_truecolor(void) {
  return vl_color_support_detect() >= VL_COLOR_SUPPORT_TRUECOLOR;
}

bool vl_color_supports_hyperlinks(void) {
  if (!g_initialized_)
    vl_color_init(VAL_COLOR_AUTO);
  else if (!g_hyperlinks_detected_) {
    g_hyperlinks_ = detect_hyperlinks_();
    g_hyperlinks_detected_ = true;
  }
  return g_hyperlinks_;
}

void vl_term_caps(vl_term_caps_t *out) {
  if (!out)
    return;
  if (!g_initialized_)
    vl_color_init(VAL_COLOR_AUTO);
  out->stdout_tty = g_stdout_tty_;
  out->stderr_tty = g_stderr_tty_;
  out->utf8 = g_utf8_;
  out->hyperlinks = vl_color_supports_hyperlinks();
  out->columns = g_columns_ ? g_columns_ : 80; /* GCOVR_EXCL_BR_LINE: probe always seeds defaults */
  out->rows = g_rows_ ? g_rows_ : 24; /* GCOVR_EXCL_BR_LINE */
  out->color = vl_color_support_detect();
}

void vl_color_support_force(vl_color_support_t level) {
  g_support_ = level;
  g_support_forced_ = true;
  g_support_detected_ = true;
  if (g_initialized_) /* GCOVR_EXCL_BR_LINE: force-before-init vs after both exercised across TUs; one arm host-order */
    apply_enable_bits_(g_mode_);
}

void vl_color_support_reset(void) {
  g_support_forced_ = false;
  if (g_initialized_)
    refresh_caps_(g_mode_);
  /* Clear after refresh so the next detect/hyperlinks query re-enters the
   * lazy refresh branches (initialized && !detected). */
  g_support_detected_ = false;
  g_hyperlinks_detected_ = false;
}

bool vl_color_enabled(FILE *stream) {
  if (!g_initialized_)
    vl_color_init(VAL_COLOR_AUTO);
  if (stream == stdout)
    return g_stdout_color_;
  if (stream == stderr)
    return g_stderr_color_;
  return false;
}

const char *vl_color_for(FILE *stream, const char *seq) {
  return vl_color_enabled(stream) ? seq : "";
}
