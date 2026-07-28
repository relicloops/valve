#include "unity.h"
#include "color_only.h"
#include <stdlib.h>
TEST_SOURCE_FILE("color.c")
#define EXPECT(cond, name) TEST_ASSERT_TRUE_MESSAGE((cond), (name))
void setUp(void) {
  unsetenv("NO_COLOR");
  unsetenv("FORCE_COLOR");
  unsetenv("TERM");
}
void tearDown(void) {
  unsetenv("FORCE_COLOR");
  unsetenv("TERM");
  vl_color_support_reset();
}
void test_lazy_term_caps_only(void) {
  setenv("FORCE_COLOR", "1", 1);
  setenv("TERM", "xterm", 1);
  vl_term_caps_t caps;
  vl_term_caps(&caps);
  EXPECT(caps.columns >= 1, "lazy caps");
}
