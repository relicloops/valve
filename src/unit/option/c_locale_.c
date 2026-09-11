#include "../../private.h"

/* Numeric conversions must not depend on the host's LC_NUMERIC, which any
 * program may switch with setlocale(). The locale is created once and kept
 * for the process; freeing it on vl_destroy would break a second parser. */
locale_t option_c_locale_(void) {
  static locale_t c_locale = (locale_t)0;

  if (!c_locale)
    c_locale = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
  return c_locale;
}
