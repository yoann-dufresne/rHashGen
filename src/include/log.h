#ifndef LOG_H
#define LOG_H

#include <clutchlog/clutchlog.h>

// Make asserts (de)clutchable.
#define ASSERT(EXPR) { CLUTCHFUNC(critical, assert, EXPR); }

/** Generic parts of the configuration for clutchlog.
 *
 * Mainly colors and line template.
 */
void clutchlog_config();

#endif // LOG_H
