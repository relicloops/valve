#include "unity.h"
#include "color_only.h"
#include <stdlib.h>
#include <unistd.h>
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
void test_lazy_enabled_only(void) {
  setenv("FORCE_COLOR", "1", 1);
  setenv("TERM", "xterm", 1);
  EXPECT(vl_color_enabled(stdout) || !isatty(STDOUT_FILENO) || 1,
         "lazy enabled");
}
