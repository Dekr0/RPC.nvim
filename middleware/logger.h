#ifndef RICH_PRESENCE_NVIM_MIDDLEWARE_LOGGER_H
#define RICH_PRESENCE_NVIM_MIDDLEWARE_LOGGER_H

#define LOG(f, m, lv) _log(f, m, lv)

enum Level { INFO = 30, WARN = 40, FATAL = 50 };

void new_logger();

void version();

void close_logger();

void flush();

void _log(const char *, const char *, int);

#endif
