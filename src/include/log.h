#pragma once

#include <clutchlog/clutchlog.h>

namespace frictionless {

// Make asserts (de)clutchable.
#define ASSERT(EXPR) { CLUTCHFUNC(critical, assert, EXPR); }

/** Generic parts of the configuration for clutchlog.
 *
 * Mainly colors and line template.
 */
//void clutchlog_config();

} // frictionless
