#include "fault_alloc.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
 * Fault injection through Ceedling's allocation-name compile definitions.
 * Countdown: fail_after(N) makes the Nth subsequent allocation return NULL.
 */

static int g_fail_after = -1;
static size_t g_alloc_count = 0;

#undef malloc
#undef calloc
#undef realloc
#undef strdup
#undef strndup

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
char *strdup(const char *s);
char *strndup(const char *s, size_t n);

void vl_test_alloc_reset(void) {
  g_fail_after = -1;
  g_alloc_count = 0;
}

void vl_test_alloc_fail_after(int n) {
  if (n <= 0) {
    g_fail_after = -1;
    return;
  }
  g_fail_after = n;
}

size_t vl_test_alloc_count(void) { return g_alloc_count; }

static int should_fail_(void) {
  if (g_fail_after < 0)
    return 0;
  if (g_fail_after == 0)
    return 1;
  --g_fail_after;
  return g_fail_after == 0;
}

void *vl_test_malloc(size_t size) {
  ++g_alloc_count;
  if (should_fail_())
    return NULL;
  return malloc(size);
}

void *vl_test_calloc(size_t nmemb, size_t size) {
  ++g_alloc_count;
  if (should_fail_())
    return NULL;
  return calloc(nmemb, size);
}

void *vl_test_realloc(void *ptr, size_t size) {
  ++g_alloc_count;
  if (should_fail_())
    return NULL;
  return realloc(ptr, size);
}

char *vl_test_strdup(const char *s) {
  ++g_alloc_count;
  if (should_fail_())
    return NULL;
  return strdup(s);
}

char *vl_test_strndup(const char *s, size_t n) {
  ++g_alloc_count;
  if (should_fail_())
    return NULL;
  return strndup(s, n);
}
