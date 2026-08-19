#include <stdio.h>

#define LOG_ERR(fmt, ...) \
    fprintf(stderr, "[DEV ERROR] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    fprintf(stderr, "[DEV WARN]  %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#ifdef NDEBUG

    #define LOG_DEBUG(fmt, ...) do { } while (0)
    #define LOG_TRACE(fmt, ...) do { } while (0)

#else

    #define LOG_DEBUG(fmt, ...) \
        fprintf(stderr, "[DEV DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

    #define LOG_TRACE(fmt, ...) \
        fprintf(stderr, "[DEV TRACE] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
