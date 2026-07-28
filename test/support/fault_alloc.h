#ifndef VALVE_FAULT_ALLOC_H
#define VALVE_FAULT_ALLOC_H

#include <stddef.h>

/** Reset countdown so allocations succeed (default). */
void vl_test_alloc_reset(void);

/**
 * Fail the Nth subsequent malloc/calloc/realloc/strdup/strndup call.
 * N=1 fails the next allocation. N<=0 is ignored (use reset).
 */
void vl_test_alloc_fail_after(int n);

/** How many real allocations have been observed since last reset. */
size_t vl_test_alloc_count(void);

void *vl_test_malloc(size_t size);
void *vl_test_calloc(size_t nmemb, size_t size);
void *vl_test_realloc(void *ptr, size_t size);
char *vl_test_strdup(const char *s);
char *vl_test_strndup(const char *s, size_t n);

#endif /* VALVE_FAULT_ALLOC_H */
