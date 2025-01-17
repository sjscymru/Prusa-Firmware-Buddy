#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool filename_has_ext(const char *fname, const char *ext);

/**
 * \brief Does the filename look like a gcode?
 *
 * Checks the extensions and decides if it is most probably a gcode file.
 */
bool filename_is_gcode(const char *fname);

#ifdef __cplusplus
}
#endif
