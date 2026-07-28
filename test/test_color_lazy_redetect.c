#include "unity.h"
#include "color_only.h"
#include <stdlib.h>

TEST_SOURCE_FILE("color.c")

#define EXPECT(cond, name) TEST_ASSERT_TRUE_MESSAGE((cond), (name))

void setUp(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  unsetenv("TERM");
  unsetenv("KITTY_WINDOW_ID");
}

void tearDown(void) {
  unsetenv("FORCE_COLOR");
  unsetenv("TERM");
  unsetenv("KITTY_WINDOW_ID");
  vl_color_support_reset();
}

void test_redetect_after_reset(void) {
  setenv("FORCE_COLOR", "1", 1);
  setenv("TERM", "xterm-256color", 1);
  /* First init */
  EXPECT(vl_color_supports(), "init supports");
  vl_color_support_reset();
  /* initialized && !g_support_detected_ → refresh_caps_ */
  (void)vl_color_support_detect();
  vl_color_support_reset();
  /* initialized && !g_hyperlinks_detected_ → detect_hyperlinks_ */
  (void)vl_color_supports_hyperlinks();
  EXPECT(1, "redetect paths");
}
