#ifndef LOGGER_H
#define	LOGGER_H

#include <defs.h>
#include <asm/libasm.h>
#include <varargs.h>
#include <stdarg.h>

enum LogLevel {L_FATAL, L_ERROR, L_INFO, L_DEBUG, L_TRACE};
#define LOG_LEVEL L_DEBUG
/**
 * @param level a level of type Level
 * @param fmt a format like printf
 * @param ... the format list of items.
 * @return 0 if logged, -1 if level is above the defined LOG_LEVEL.
 */
int log(enum LogLevel level, char *fmt, ...);

#endif	/* LOGGER_H */

