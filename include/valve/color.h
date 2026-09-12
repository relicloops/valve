#ifndef VALVE_COLOR_H
#define VALVE_COLOR_H

#include <stdbool.h>
#include <stdio.h>

/* ---- ANSI SGR escape codes ---- */

#define VAL_RESET      "\033[0m"

#define VAL_BOLD       "\033[1m"
#define VAL_DIM        "\033[2m"
#define VAL_ITALIC     "\033[3m"
#define VAL_UNDERLINE  "\033[4m"

#define VAL_FG_BLACK   "\033[30m"
#define VAL_FG_RED     "\033[31m"
#define VAL_FG_GREEN   "\033[32m"
#define VAL_FG_YELLOW  "\033[33m"
#define VAL_FG_BLUE    "\033[34m"
#define VAL_FG_MAGENTA "\033[35m"
#define VAL_FG_CYAN    "\033[36m"
#define VAL_FG_WHITE   "\033[37m"

#define VAL_FG_BRIGHT_BLACK   "\033[90m"
#define VAL_FG_BRIGHT_RED     "\033[91m"
#define VAL_FG_BRIGHT_GREEN   "\033[92m"
#define VAL_FG_BRIGHT_YELLOW  "\033[93m"
#define VAL_FG_BRIGHT_BLUE    "\033[94m"
#define VAL_FG_BRIGHT_MAGENTA "\033[95m"
#define VAL_FG_BRIGHT_CYAN    "\033[96m"
#define VAL_FG_BRIGHT_WHITE   "\033[97m"

/* ---- runtime color control ---- */

typedef enum vl_color_mode {
  VAL_COLOR_AUTO, /* detect from terminal */
  VAL_COLOR_ALWAYS,
  VAL_COLOR_NEVER
} vl_color_mode_t;

/** Ordered color capability. Helpers use >= (TRUECOLOR implies 256, etc.). */
typedef enum vl_color_support {
  VL_COLOR_SUPPORT_NONE = 0,  /* no SGR color */
  VL_COLOR_SUPPORT_BASIC,     /* 8/16 */
  VL_COLOR_SUPPORT_256,       /* 256 */
  VL_COLOR_SUPPORT_TRUECOLOR, /* 24-bit RGB */
} vl_color_support_t;

/** Cached terminal capability snapshot (Linux / macOS). */
typedef struct vl_term_caps {
  bool stdout_tty;
  bool stderr_tty;
  bool utf8;
  bool hyperlinks;
  unsigned columns; /* 0 → treat as 80 */
  unsigned rows;    /* 0 → treat as 24 */
  vl_color_support_t color;
} vl_term_caps_t;

/**
 * Call once at startup to pin the preferred mode. Optional: the first color
 * query lazily runs AUTO detection when init was never called.
 *
 * Precedence:
 *   1. NO_COLOR non-empty              → NONE
 *   2. FORCE_COLOR leveled 0/1/2/3     → NONE/BASIC/256/TRUECOLOR
 *   3. mode NEVER                      → NONE
 *   4. non-TTY stream                  → NONE for that stream's enable bit
 *   5. COLORTERM / TERM_PROGRAM / TERM heuristics (see detect)
 *   6. mode ALWAYS floors to at least BASIC when TTY and TERM is not dumb
 *
 * vl_create applies settings->color (default AUTO when unset / zero).
 */
void vl_color_init(vl_color_mode_t mode);

/** Returns true when color is enabled for the given stream (stdout/stderr). */
bool vl_color_enabled(FILE *stream);

/** Returns `seq` when color is enabled for `stream`, otherwise "". */
const char *vl_color_for(FILE *stream, const char *seq);

/** Cached color support level (runs detect once unless forced/reset). */
vl_color_support_t vl_color_support_detect(void);

bool vl_color_supports(void);
bool vl_color_supports_256(void);
bool vl_color_supports_truecolor(void);
bool vl_color_supports_hyperlinks(void);

/** Fill a snapshot of probed capabilities (runs init/detect as needed). */
void vl_term_caps(vl_term_caps_t *out);

/** Test helpers: pin / clear a forced support level. */
void vl_color_support_force(vl_color_support_t level);
void vl_color_support_reset(void);

#endif /* VALVE_COLOR_H */
