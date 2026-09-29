#include "../private.h"

#include <stdio.h>
#include <string.h>

int vl_parse_kv_value_(valve_t *v, const vl_option_t *opt, const char *raw, int argv_index) {

  vl_value_t value = {
    .kind = VL_VALUE_KV,
    .raw = strdup(raw),
  };
  if (!value.raw) {
    return -1;
  }

  size_t error_at = 0;

  option_kv_status_t status = option_kv_parse_(raw, &value.as.kv, &error_at);
  if (status == OPTION_KV_OUT_OF_MEMORY) {
    vl_value_clear(&value);

    return -1;
  }

  if (status != OPTION_KV_OK) {
    char message[96];

    snprintf(message, sizeof message, "%s at offset %zu", option_kv_message_(status), error_at);
    vl_value_clear(&value);

    return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, message);
  }

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);

    return -1;
  }

  return 0;
}
