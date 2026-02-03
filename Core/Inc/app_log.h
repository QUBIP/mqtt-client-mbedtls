#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdio.h>

typedef enum {
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
} log_level_t;

void Log_Init(void);
void Log_Print(log_level_t level, const char *tag, const char *fmt, ...);

#define LOGE(tag, fmt, ...) Log_Print(LOG_ERROR, tag, fmt, ##__VA_ARGS__)
#define LOGW(tag, fmt, ...) Log_Print(LOG_WARN,  tag, fmt, ##__VA_ARGS__)
#define LOGI(tag, fmt, ...) Log_Print(LOG_INFO,  tag, fmt, ##__VA_ARGS__)
#define LOGD(tag, fmt, ...) Log_Print(LOG_DEBUG, tag, fmt, ##__VA_ARGS__)

#endif
