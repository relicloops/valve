#ifndef VALVE_PTY_COMPAT_H
#define VALVE_PTY_COMPAT_H

/*
 * openpty is portable; the header is not.
 * - macOS / BSD: <util.h>
 * - Linux: <pty.h> (link with -lutil)
 */
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__NetBSD__)
#include <util.h>
#elif defined(__linux__)
#include <pty.h>
#endif

#endif /* VALVE_PTY_COMPAT_H */
