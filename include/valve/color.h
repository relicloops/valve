/**
 * @file color.h
 * ANSI SGR escape sequences and runtime terminal capability detection.
 *
 * The module keeps one process-wide snapshot of color mode, per-stream
 * enable bits and probed terminal capabilities. Every query lazily runs
 * AUTO detection when vl_color_init() was never called, so callers may use
 * the helpers without any setup. None of the functions allocate; returned
 * strings are either the caller's `seq` or a static "".
 *
 * Only stdout and stderr are tracked. Any other `FILE *` is reported as
 * color-disabled.
 */

#ifndef VALVE_COLOR_H
#define VALVE_COLOR_H

#include <stdbool.h>
#include <stdio.h>

/* ---- ANSI SGR escape codes ---- */

/** Reset every SGR attribute (colors, bold, dim, italic, underline). */
#define VAL_RESET      "\033[0m"

#define VAL_BOLD       "\033[1m" /**< SGR 1: bold / increased intensity */
#define VAL_DIM        "\033[2m" /**< SGR 2: faint / decreased intensity */
#define VAL_ITALIC     "\033[3m" /**< SGR 3: italic (not universally supported) */
#define VAL_UNDERLINE  "\033[4m" /**< SGR 4: single underline */

#define VAL_FG_BLACK   "\033[30m" /**< SGR 30: foreground black */
#define VAL_FG_RED     "\033[31m" /**< SGR 31: foreground red */
#define VAL_FG_GREEN   "\033[32m" /**< SGR 32: foreground green */
#define VAL_FG_YELLOW  "\033[33m" /**< SGR 33: foreground yellow */
#define VAL_FG_BLUE    "\033[34m" /**< SGR 34: foreground blue */
#define VAL_FG_MAGENTA "\033[35m" /**< SGR 35: foreground magenta */
#define VAL_FG_CYAN    "\033[36m" /**< SGR 36: foreground cyan */
#define VAL_FG_WHITE   "\033[37m" /**< SGR 37: foreground white */

#define VAL_FG_BRIGHT_BLACK   "\033[90m" /**< SGR 90: bright black (grey) */
#define VAL_FG_BRIGHT_RED     "\033[91m" /**< SGR 91: bright red */
#define VAL_FG_BRIGHT_GREEN   "\033[92m" /**< SGR 92: bright green */
#define VAL_FG_BRIGHT_YELLOW  "\033[93m" /**< SGR 93: bright yellow */
#define VAL_FG_BRIGHT_BLUE    "\033[94m" /**< SGR 94: bright blue */
#define VAL_FG_BRIGHT_MAGENTA "\033[95m" /**< SGR 95: bright magenta */
#define VAL_FG_BRIGHT_CYAN    "\033[96m" /**< SGR 96: bright cyan */
#define VAL_FG_BRIGHT_WHITE   "\033[97m" /**< SGR 97: bright white */

/* ---- runtime color control ---- */

/** Caller preference for color output, applied by vl_color_init().
 *  Environment overrides (`NO_COLOR`, `FORCE_COLOR`) take precedence over
 *  every mode; see vl_color_init() for the full order. */
typedef enum vl_color_mode {
  VAL_COLOR_AUTO,   /**< enable color only on a TTY whose TERM advertises it */
  VAL_COLOR_ALWAYS, /**< enable color even when the stream is not a TTY;
                     *   floors support to BASIC when TERM is empty */
  VAL_COLOR_NEVER   /**< never emit color unless FORCE_COLOR forces it */
} vl_color_mode_t;

/** Ordered color capability. Helpers use >= (TRUECOLOR implies 256, etc.). */
typedef enum vl_color_support {
  VL_COLOR_SUPPORT_NONE = 0,  /**< no SGR color */
  VL_COLOR_SUPPORT_BASIC,     /**< 8/16 colors (SGR 30-37, 90-97) */
  VL_COLOR_SUPPORT_256,       /**< 256-color palette (SGR 38;5;n) */
  VL_COLOR_SUPPORT_TRUECOLOR, /**< 24-bit RGB (SGR 38;2;r;g;b) */
} vl_color_support_t;

/** Cached terminal capability snapshot (Linux / macOS). Filled by
 *  vl_term_caps(); the fields mirror the module's internal state at the
 *  time of the call and are not updated afterwards. */
typedef struct vl_term_caps {
  bool stdout_tty;  /**< isatty(STDOUT_FILENO) */
  bool stderr_tty;  /**< isatty(STDERR_FILENO) */
  bool utf8;        /**< locale codeset (or LC_ALL / LC_CTYPE / LANG) is UTF-8 */
  bool hyperlinks;  /**< OSC 8 hyperlinks are likely supported */
  unsigned columns; /**< terminal width; 0 → treat as 80 */
  unsigned rows;    /**< terminal height; 0 → treat as 24 */
  vl_color_support_t color; /**< detected (or forced) color level */
} vl_term_caps_t;

/**
 * Pin the preferred color mode and (re)probe the terminal.
 *
 * Call once at startup. Optional: the first color query lazily runs AUTO
 * detection when init was never called. Calling it again re-runs every
 * probe (TTY, UTF-8, window size, color, hyperlinks) under the new mode;
 * a level pinned with vl_color_support_force() survives re-init.
 *
 * Precedence:
 *   1. NO_COLOR non-empty              → NONE
 *   2. FORCE_COLOR leveled 0/1/2/3     → NONE/BASIC/256/TRUECOLOR
 *                                        (any other non-empty value → TRUECOLOR)
 *   3. mode NEVER                      → NONE
 *   4. no TTY on stdout or stderr      → NONE
 *   5. COLORTERM / TERM_PROGRAM / TERM heuristics
 *   6. mode ALWAYS floors to at least BASIC when TTY and TERM is empty;
 *      TERM=dumb stays NONE
 *
 * The per-stream enable bit is then set when the level is >= BASIC and the
 * stream is a TTY, FORCE_COLOR is on, or the mode is ALWAYS.
 *
 * vl_create() applies settings->color (default AUTO when unset / zero).
 *
 * @param mode Preferred mode; see vl_color_mode_t.
 */
void vl_color_init(vl_color_mode_t mode);

/**
 * Report whether color output is enabled for a stream.
 *
 * Runs lazy AUTO init when needed.
 *
 * @param stream `stdout` or `stderr`; any other pointer (including NULL)
 *               yields false.
 * @return true when SGR sequences should be emitted on `stream`.
 */
bool vl_color_enabled(FILE *stream);

/**
 * Select an escape sequence for a stream.
 *
 * Convenience for `printf("%s...%s", vl_color_for(f, VAL_BOLD),
 * vl_color_for(f, VAL_RESET))`. Runs lazy AUTO init when needed.
 *
 * @param stream `stdout` or `stderr` (see vl_color_enabled()).
 * @param seq    Escape sequence to return when color is enabled; not
 *               copied, so it must outlive the returned pointer.
 * @return `seq` when color is enabled for `stream`, otherwise a static "".
 */
const char *vl_color_for(FILE *stream, const char *seq);

/**
 * Cached color support level.
 *
 * Runs detection once (lazily initialising with AUTO when needed) and then
 * returns the cached value until vl_color_init() or
 * vl_color_support_reset() invalidates it. A level pinned with
 * vl_color_support_force() is returned as-is without probing.
 *
 * @return The detected or forced vl_color_support_t.
 */
vl_color_support_t vl_color_support_detect(void);

/** @return true when vl_color_support_detect() >= VL_COLOR_SUPPORT_BASIC. */
bool vl_color_supports(void);

/** @return true when vl_color_support_detect() >= VL_COLOR_SUPPORT_256. */
bool vl_color_supports_256(void);

/** @return true when vl_color_support_detect() >= VL_COLOR_SUPPORT_TRUECOLOR. */
bool vl_color_supports_truecolor(void);

/**
 * Report whether OSC 8 hyperlinks are likely supported.
 *
 * True for known terminal programs (iTerm.app, Hyper, vscode), VTE, kitty,
 * WezTerm and Konsole, or whenever the color level is TRUECOLOR. Cached
 * after the first probe; runs lazy AUTO init when needed.
 *
 * @return true when emitting hyperlink escapes is reasonable.
 */
bool vl_color_supports_hyperlinks(void);

/**
 * Fill a snapshot of probed capabilities.
 *
 * Runs lazy AUTO init and any pending color / hyperlink detection as needed.
 *
 * @param out Destination; NULL is ignored. `columns` and `rows` are never
 *            0 in the filled snapshot (defaults 80 / 24 apply).
 */
void vl_term_caps(vl_term_caps_t *out);

/**
 * Test helper: pin a forced support level.
 *
 * Bypasses environment and TTY detection for vl_color_support_detect() and
 * the `vl_color_supports*` helpers, and recomputes the per-stream enable
 * bits when the module is already initialised. The pin survives
 * vl_color_init() until vl_color_support_reset() is called.
 *
 * @param level Level to report.
 */
void vl_color_support_force(vl_color_support_t level);

/**
 * Test helper: clear a forced support level.
 *
 * Re-probes the terminal under the current mode when already initialised
 * and marks color / hyperlink detection stale so the next query re-runs it.
 */
void vl_color_support_reset(void);

#endif /* VALVE_COLOR_H */
