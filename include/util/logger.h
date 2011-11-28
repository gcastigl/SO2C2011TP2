#ifndef LOGGER_H
#define	LOGGER_H

#include <defs.h>
#include <varargs.h>
#include <stdarg.h>
#include <driver/port.h>

enum LogLevel {L_FATAL, L_ERROR, L_INFO, L_DEBUG, L_TRACE};
#define LOG_LEVEL L_DEBUG
/**
 * @param level a level of type Level
 * @param fmt a format like printf
 * @param ... the format list of items.
 * @return 0 if logged, -1 if level is above the defined LOG_LEVEL.
 */
#define log(...) _log(__FILE__, __LINE__, __VA_ARGS__)
#define ASSERT(b) ((b) ? (void)0 : (log(L_ERROR, "There was an assertion error.")))

int _log(char* file, int line, enum LogLevel level, char *formatString, ...);

#endif	/* LOGGER_H */

