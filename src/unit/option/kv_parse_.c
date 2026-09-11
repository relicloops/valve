#include "../../private.h"

#include <stdlib.h>
#include <string.h>

#define KV_MAX_DEPTH 32

/* Grammar (whitespace allowed around every separator, never inside a token):
 *   map   := pair ( '|' pair )*
 *   pair  := key ':' value
 *   key   := quoted | bare-key
 *   value := '{' map '}' | quoted | bare-value
 * A quoted token is "..." with \" and \\ as the only escapes. A bare key
 * stops at ':' '|' '{' '}' '"' or whitespace; a bare value additionally
 * admits ':' so URLs and times need no quotes. */

typedef struct kv_cursor {
  const char *base;
  const char *p;
} kv_cursor_t;

static size_t offset_(const kv_cursor_t *c) { return (size_t)(c->p - c->base); }

static void skip_ws_(kv_cursor_t *c) {
  while (*c->p == ' ' || *c->p == '\t')
    ++c->p;
}

static bool bare_key_char_(char ch) {
  return ch != '\0' && ch != ':' && ch != '|' && ch != '{' && ch != '}' &&
         ch != '"' && ch != ' ' && ch != '\t';
}

static bool bare_value_char_(char ch) {
  return ch != '\0' && ch != '|' && ch != '{' && ch != '}' && ch != '"' &&
         ch != ' ' && ch != '\t';
}

static int pair_push_(vl_kv_list_t *list, vl_kv_pair_t *pair) {
  size_t next_count, bytes;
  /* GCOVR_EXCL_BR_START — size_t wrap on pathological count */
  if (__builtin_add_overflow(list->count, (size_t)1, &next_count) ||
      __builtin_mul_overflow(next_count, sizeof(vl_kv_pair_t), &bytes))
    return -1; /* GCOVR_EXCL_LINE */
  /* GCOVR_EXCL_BR_STOP */

  vl_kv_pair_t *next = realloc((vl_kv_pair_t *)list->pairs, bytes);

  if (!next)
    return -1;

  next[list->count] = *pair;
  list->pairs = next;
  ++list->count;
  *pair = (vl_kv_pair_t){0};
  return 0;
}

static bool has_key_(const vl_kv_list_t *list, const char *key) {
  for (size_t i = 0; i < list->count; ++i) {
    if (strcmp(list->pairs[i].key, key) == 0)
      return true;
  }
  return false;
}

/* Cursor on the opening quote. Validates and measures first, then copies
 * with escapes resolved, so a malformed string allocates nothing. */
static option_kv_status_t parse_quoted_(kv_cursor_t *c, char **out,
                                        size_t *error_at) {
  size_t opened_at = offset_(c);
  const char *start = c->p + 1;
  const char *q = start;
  size_t length = 0;

  for (;; ++q) {
    if (*q == '\0') {
      *error_at = opened_at;
      return OPTION_KV_QUOTE;
    }
    if (*q == '"')
      break;
    if (*q == '\\') {
      ++q;
      if (*q != '"' && *q != '\\') {
        *error_at = (size_t)(q - c->base);
        return OPTION_KV_ESCAPE;
      }
    }
    ++length;
  }

  char *copy = malloc(length + 1);
  if (!copy)
    return OPTION_KV_OUT_OF_MEMORY;

  size_t i = 0;
  for (const char *r = start; r < q; ++r) {
    if (*r == '\\')
      ++r;
    copy[i++] = *r;
  }
  copy[length] = '\0';

  *out = copy;
  c->p = q + 1;
  return OPTION_KV_OK;
}

static option_kv_status_t parse_key_(kv_cursor_t *c, char **key,
                                     size_t *error_at) {
  if (*c->p == '"')
    return parse_quoted_(c, key, error_at);

  const char *start = c->p;
  while (bare_key_char_(*c->p))
    ++c->p;
  if (c->p == start) {
    *error_at = offset_(c);
    return OPTION_KV_KEY;
  }

  *key = strndup(start, (size_t)(c->p - start));
  return *key ? OPTION_KV_OK : OPTION_KV_OUT_OF_MEMORY;
}

static option_kv_status_t parse_map_(kv_cursor_t *c, vl_kv_list_t *list,
                                     bool nested, int depth, size_t *error_at);

static option_kv_status_t parse_value_(kv_cursor_t *c, vl_value_t *value,
                                       int depth, size_t *error_at) {
  if (*c->p == '{') {
    ++c->p;
    value->kind = VL_VALUE_KV;
    value->raw = nullptr;
    return parse_map_(c, &value->as.kv, true, depth + 1, error_at);
  }

  if (*c->p == '"') {
    char *text = nullptr;
    option_kv_status_t status = parse_quoted_(c, &text, error_at);

    if (status != OPTION_KV_OK)
      return status;
    value->kind = VL_VALUE_STRING;
    value->raw = text;
    return OPTION_KV_OK;
  }

  const char *start = c->p;
  while (bare_value_char_(*c->p))
    ++c->p;
  if (c->p == start) {
    *error_at = offset_(c);
    return OPTION_KV_VALUE;
  }

  value->raw = strndup(start, (size_t)(c->p - start));
  if (!value->raw)
    return OPTION_KV_OUT_OF_MEMORY;

  switch (option_scalar_auto_(value)) { /* GCOVR_EXCL_BR_LINE: default arm unreachable, classifier returns only the three statuses */
  case OPTION_SCALAR_INT_RANGE:
    *error_at = (size_t)(start - c->base);
    return OPTION_KV_INT_RANGE;
  case OPTION_SCALAR_DOUBLE_RANGE:
    *error_at = (size_t)(start - c->base);
    return OPTION_KV_DOUBLE_RANGE;
  case OPTION_SCALAR_OK:
  default:
    return OPTION_KV_OK;
  }
}

static option_kv_status_t parse_map_(kv_cursor_t *c, vl_kv_list_t *list,
                                     bool nested, int depth, size_t *error_at) {
  if (depth > KV_MAX_DEPTH) {
    *error_at = offset_(c);
    return OPTION_KV_DEPTH;
  }

  /* 1.x literals started every map with '!'. Fail loudly with a hint
   * rather than parsing "!a" as a key. */
  skip_ws_(c);
  if (*c->p == '!') {
    *error_at = offset_(c);
    return OPTION_KV_BANG;
  }

  for (;;) {
    vl_kv_pair_t pair = {0};
    option_kv_status_t status;

    skip_ws_(c);
    size_t key_at = offset_(c);

    status = parse_key_(c, (char **)&pair.key, error_at);
    if (status != OPTION_KV_OK)
      return status;
    if (has_key_(list, pair.key)) {
      free((char *)pair.key);
      *error_at = key_at;
      return OPTION_KV_DUPLICATE;
    }

    skip_ws_(c);
    if (*c->p != ':') {
      free((char *)pair.key);
      *error_at = offset_(c);
      return OPTION_KV_COLON;
    }
    ++c->p;
    skip_ws_(c);

    status = parse_value_(c, &pair.value, depth, error_at);
    if (status != OPTION_KV_OK) {
      free((char *)pair.key);
      vl_value_clear(&pair.value);
      return status;
    }

    if (pair_push_(list, &pair) != 0) {
      free((char *)pair.key);
      vl_value_clear(&pair.value);
      return OPTION_KV_OUT_OF_MEMORY;
    }

    skip_ws_(c);
    if (*c->p != '|')
      break;
    ++c->p;
  }

  if (nested) {
    if (*c->p != '}') {
      *error_at = offset_(c);
      return OPTION_KV_BRACE;
    }
    ++c->p;
    return OPTION_KV_OK;
  }

  if (*c->p != '\0') {
    *error_at = offset_(c);
    return OPTION_KV_SEPARATOR;
  }
  return OPTION_KV_OK;
}

option_kv_status_t option_kv_parse_(const char *raw, vl_kv_list_t *list,
                                    size_t *error_at) {
  kv_cursor_t cursor = {.base = raw, .p = raw};

  *error_at = 0;
  return parse_map_(&cursor, list, false, 1, error_at);
}

const char *option_kv_message_(option_kv_status_t status) {
  switch (status) {
  case OPTION_KV_KEY:
    return "expected key";
  case OPTION_KV_COLON:
    return "expected ':' after key";
  case OPTION_KV_VALUE:
    return "expected value";
  case OPTION_KV_QUOTE:
    return "unterminated quoted string";
  case OPTION_KV_ESCAPE:
    return "unknown escape, only \\\" and \\\\ are allowed";
  case OPTION_KV_SEPARATOR:
    return "expected '|' or end of value";
  case OPTION_KV_BRACE:
    return "expected '|' or '}'";
  case OPTION_KV_DEPTH:
    return "nesting deeper than 32 levels";
  case OPTION_KV_DUPLICATE:
    return "duplicate key";
  case OPTION_KV_INT_RANGE:
    return "integer value does not fit 64 bits";
  case OPTION_KV_DOUBLE_RANGE:
    return "decimal value is out of range";
  case OPTION_KV_BANG:
    return "the '!' prefix was removed in 2.0, write key:value";
  case OPTION_KV_OUT_OF_MEMORY:
    return "out of memory";
  case OPTION_KV_OK:
  default:
    return "expected key:value|key:{nested:value}";
  }
}
