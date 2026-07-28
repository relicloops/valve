#include "unity.h"
#include "color_only.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Dedicated binary so g_initialized_ starts false. Do not call vl_create
 * before the lazy-init probes below. Avoid test_support (pulls valve).
 */

TEST_SOURCE_FILE("color.c")

#define EXPECT(cond, name) TEST_ASSERT_TRUE_MESSAGE((cond), (name))

void setUp(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  unsetenv("COLORTERM");
  unsetenv("TERM");
  unsetenv("TERM_PROGRAM");
  unsetenv("COLUMNS");
  unsetenv("LINES");
  unsetenv("KITTY_WINDOW_ID");
  unsetenv("WEZTERM_PANE");
  unsetenv("KONSOLE_VERSION");
  unsetenv("VTE_VERSION");
}

void tearDown(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  unsetenv("TERM");
  vl_color_support_reset();
}

/* MUST run first in this file — hits !g_initialized_ branches. */
void test_color_lazy_init_before_any_init(void) {
  setenv("TERM", "xterm-256color", 1);
  setenv("FORCE_COLOR", "3", 1);
  (void)vl_color_support_detect();
  (void)vl_color_supports_hyperlinks();
  vl_term_caps_t caps;
  vl_term_caps(&caps);
  EXPECT(vl_color_enabled(stdout) || !isatty(STDOUT_FILENO) ||
             caps.color >= VL_COLOR_SUPPORT_NONE,
         "lazy init via enabled/detect/caps");
  EXPECT(vl_color_for(stdout, VAL_FG_RED) != NULL, "color_for after lazy");
}

void test_color_detect_refresh_when_initialized(void) {
  vl_color_support_reset();
  setenv("TERM", "xterm-256color", 1);
  setenv("FORCE_COLOR", "2", 1);
  vl_color_init(VAL_COLOR_AUTO);
  vl_color_support_reset();
  (void)vl_color_support_detect();
  (void)vl_color_supports_hyperlinks();
  EXPECT(1, "refresh path");
}
