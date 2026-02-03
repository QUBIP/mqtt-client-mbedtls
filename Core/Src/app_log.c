#include "app_log.h"
#include "stm32f4xx_hal.h"
#include <stdarg.h>

extern UART_HandleTypeDef huart3;

static uint32_t start_tick;

void Log_Init(void)
{
    start_tick = HAL_GetTick();
}

void Log_Print(log_level_t level, const char *tag, const char *fmt, ...)
{
    const char *lvl_str[] = { "ERR", "WRN", "INF", "DBG" };
    char buf[128];

    uint32_t ms = HAL_GetTick() - start_tick;

    int len = snprintf(buf, sizeof(buf),
        "[%8lu ms] [%s] [%s] ",
        ms, lvl_str[level], tag);

    va_list args;
    va_start(args, fmt);
    len += vsnprintf(buf + len, sizeof(buf) - len, fmt, args);
    va_end(args);

    buf[len++] = '\r';
    buf[len++] = '\n';

    HAL_UART_Transmit(&huart3, (uint8_t*)buf, len, HAL_MAX_DELAY);
}
