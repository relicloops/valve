#ifndef VALVE_TEST_SUPPORT_H
#define VALVE_TEST_SUPPORT_H

#include "unity.h"

#include "color.h"
#include "valve.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VL_OPT(...) (&(const vl_option_t){__VA_ARGS__})
#define VL_CMD(...) (&(const vl_verb_t){__VA_ARGS__})

/** Assert `cond`; `name` is the failure message (legacy Meson EXPECT). */
#define EXPECT(cond, name) TEST_ASSERT_TRUE_MESSAGE((cond), (name))

valve_t *parser_(const vl_option_t *const *options, size_t option_count);
valve_t *parser_with_form_(const vl_option_t *const *options,
                           size_t option_count, vl_assign_t assign);

#endif /* VALVE_TEST_SUPPORT_H */
