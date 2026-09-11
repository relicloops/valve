#include "../../private.h"

#include <errno.h>
#include <stdlib.h>

double option_strtod_c_(const char *raw, char **end) {
  locale_t c_locale = option_c_locale_();

  if (!c_locale) /* GCOVR_EXCL_BR_LINE: newlocale failure */
    return strtod(raw, end); /* GCOVR_EXCL_LINE */

  locale_t previous = uselocale(c_locale);
  double value = strtod(raw, end);
  int saved_errno = errno;

  uselocale(previous);
  errno = saved_errno;
  return value;
}
