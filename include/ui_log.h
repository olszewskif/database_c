#ifndef UI_LOG_H
#define UI_LOG_H

#include <stdio.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

#define UI_SUCCESS(fmt, ...) fprintf(stdout, COLOR_GREEN "[SUCCESS] " COLOR_RESET fmt "\n", ##__VA_ARGS__)
#define UI_ERROR(fmt, ...)   fprintf(stderr, COLOR_RED   "[ERROR]   " COLOR_RESET fmt "\n", ##__VA_ARGS__)
#define UI_WARN(fmt, ...)    fprintf(stdout, COLOR_YELLOW"[WARNING] " COLOR_RESET fmt "\n", ##__VA_ARGS__)
#define UI_INFO(fmt, ...)    fprintf(stdout, "[INFO]    " fmt "\n", ##__VA_ARGS__)

#endif
