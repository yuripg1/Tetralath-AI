#include <stdarg.h>
#include <stdio.h>

#include "debug.h"

void debug_log(const char *message, ...) {
    FILE *debug_file = fopen("debug.txt", "a");
    if (debug_file == NULL) {
        return;
    }
    va_list arguments;
    va_start(arguments, message);
    vfprintf(debug_file, message, arguments);
    va_end(arguments);
    fclose(debug_file);
}
