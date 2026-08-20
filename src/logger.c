#include "../include/logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static void log_message(const char* level, const char* fmt, va_list args) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timebuf[20];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", tm_info);

    printf("[%s] [%s] ", timebuf, level);
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);
}

void log_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("INFO", fmt, args);
    va_end(args);
}

void log_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("WARN", fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("ERROR", fmt, args);
    va_end(args);
}