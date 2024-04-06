#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "middleware_config.h"

FILE *logger;

const char ns[] = "richpresence.nvim.middleware";

void new_logger() {
    char buf[128];

    if (logger != NULL) {
        return;
    }

    logger = fopen("middleware.log", "w+");

    if (logger == NULL) {
        perror("richpresence.nvim.middleware: failed: to create logger - fopen error");
        exit(EXIT_FAILURE);
    }

    sprintf(buf, "%s VERSION %d.%d\n", "Middleware", Middleware_VERSION_MAJOR, 
            Middleware_VERSION_MINOR);
    _log("main", buf, INFO);
}

void close_logger() {
    fclose(logger);
}

inline void version() {
    printf("%s VERSION %d.%d\n", "Middleware", Middleware_VERSION_MAJOR,
            Middleware_VERSION_MINOR);
}

void flush() {
    if (fflush(logger) == EOF) {
        perror("richpresence.nvim.middleware: failed to flush - fflush error.");
    }
}

void _log(const char *f, const char *m, int lv) {
    switch (lv) {
        case INFO:
        case WARN: {
            fprintf(logger, "(%d) %s.%s: %s\n", lv, ns, f, m);
            break;
        }
        case FATAL: {
            fprintf(logger, "(%d) %s.%s: %s %s\n", lv, ns, f, m,
                    strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    flush();
}
