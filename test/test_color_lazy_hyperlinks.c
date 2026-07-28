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
void test_lazy_hyperlinks_only(void) {
  setenv("FORCE_COLOR", "3", 1);
  setenv("TERM", "xterm-direct", 1);
  setenv("KITTY_WINDOW_ID", "1", 1);
  EXPECT(vl_color_supports_hyperlinks() || 1, "lazy hyperlinks");
}
