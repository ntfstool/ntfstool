/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_LOGGER_H
#define NTFSDRV_NTFS_LOGGER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include "ntfs_core_types.h"

/* Function prototypes for log handlers */
typedef int (ntfsdrv_log_handler)(const char *function, const char *file, int line,
	u32 level, void *data, const char *format, va_list args);

/* Set the log handler to one of the following. */
void ntfsdrv_log_set_handler(ntfsdrv_log_handler *handler 
			  __attribute__((format(printf, 6, 0))));

/* List of log handlers */
ntfsdrv_log_handler ntfsdrv_log_handler_syslog  __attribute__((format(printf, 6, 0)));
ntfsdrv_log_handler ntfsdrv_log_handler_fprintf __attribute__((format(printf, 6, 0)));
ntfsdrv_log_handler ntfsdrv_log_handler_null    __attribute__((format(printf, 6, 0)));
ntfsdrv_log_handler ntfsdrv_log_handler_stdout  __attribute__((format(printf, 6, 0)));
ntfsdrv_log_handler ntfsdrv_log_handler_outerr  __attribute__((format(printf, 6, 0)));
ntfsdrv_log_handler ntfsdrv_log_handler_stderr  __attribute__((format(printf, 6, 0)));

/* Enable/disable specific log levels */
u32 ntfsdrv_log_set_levels(u32 levels);
u32 ntfsdrv_log_clear_levels(u32 levels);
u32 ntfsdrv_log_get_levels(void);

/* Parse command-line options into log flags */
BOOL ntfsdrv_log_parse_option(const char *option);

int ntfsdrv_log_redirect(const char *function, const char *file, int line,
	u32 level, void *data, const char *format, ...)
	__attribute__((format(printf, 6, 7)));

/* Log levels - determine which logs are recorded */
#define NTFS_LOG_LEVEL_DEBUG	(1 <<  0) /* Debug: x = 42 */
#define NTFS_LOG_LEVEL_TRACE	(1 <<  1) /* Trace: entering function x() */
#define NTFS_LOG_LEVEL_QUIET	(1 <<  2) /* Quiet output */
#define NTFS_LOG_LEVEL_INFO	(1 <<  3) /* Info: volume needs defragmenting */
#define NTFS_LOG_LEVEL_VERBOSE	(1 <<  4) /* Verbose: forcing continue */
#define NTFS_LOG_LEVEL_PROGRESS	(1 <<  5) /* Progress: 54% complete */
#define NTFS_LOG_LEVEL_WARNING	(1 <<  6) /* Warning: should back up before starting */
#define NTFS_LOG_LEVEL_ERROR	(1 <<  7) /* Error: operation failed, no damage done */
#define NTFS_LOG_LEVEL_PERROR	(1 <<  8) /* System error: message : standard error description */
#define NTFS_LOG_LEVEL_CRITICAL	(1 <<  9) /* Critical: operation failed, damage may have occurred */
#define NTFS_LOG_LEVEL_ENTER	(1 << 10) /* Enter function */
#define NTFS_LOG_LEVEL_LEAVE	(1 << 11) /* Leave function */

/* Log format flags - govern output style */
#define NTFS_LOG_FLAG_PREFIX	(1 << 0) /* Message prefix, e.g. "ERROR: " */
#define NTFS_LOG_FLAG_FILENAME	(1 << 1) /* Show the file name the message comes from */
#define NTFS_LOG_FLAG_LINE	(1 << 2) /* Show the line number the message comes from */
#define NTFS_LOG_FLAG_FUNCTION	(1 << 3) /* Show the name of the function containing the message */
#define NTFS_LOG_FLAG_ONLYNAME	(1 << 4) /* Show only the file name, not the path */

/* Macros to simplify logging. One for each level defined above.
 * Note: ntfsdrv_log_debug/trace only take effect when DEBUG is defined.
 */
#define ntfsdrv_log_critical(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_CRITICAL,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_error(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_ERROR,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_info(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_INFO,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_perror(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_PERROR,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_progress(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_PROGRESS,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_quiet(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_QUIET,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_verbose(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_VERBOSE,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_warning(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_WARNING,NULL,FORMAT,##ARGS)

/* By default debug and trace messages are compiled in but not shown. */
#ifdef DEBUG
#define ntfsdrv_log_debug(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_DEBUG,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_trace(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_TRACE,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_enter(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_ENTER,NULL,FORMAT,##ARGS)
#define ntfsdrv_log_leave(FORMAT, ARGS...) ntfsdrv_log_redirect(__FUNCTION__,__FILE__,__LINE__,NTFS_LOG_LEVEL_LEAVE,NULL,FORMAT,##ARGS)
#else
#define ntfsdrv_log_debug(FORMAT, ARGS...)do {} while (0)
#define ntfsdrv_log_trace(FORMAT, ARGS...)do {} while (0)
#define ntfsdrv_log_enter(FORMAT, ARGS...)do {} while (0)
#define ntfsdrv_log_leave(FORMAT, ARGS...)do {} while (0)
#endif /* DEBUG */

void ntfsdrv_log_early_error(const char *format, ...)
                __attribute__((format(printf, 1, 2)));

#endif /* NTFSDRV_NTFS_LOGGER_H */

