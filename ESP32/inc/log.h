#ifndef __LOG_H__
#define __LOG_H__

/* Logs */
#include "esp_log.h"
#define DEBUG(format, ...)	ESP_LOGD(__func__, format, ##__VA_ARGS__)
#define INFO(format, ...)	ESP_LOGI(__func__, format, ##__VA_ARGS__)
#define WARN(format, ...)	ESP_LOGW(__func__, format, ##__VA_ARGS__)
#define ERROR(format, ...)	ESP_LOGE(__func__, format, ##__VA_ARGS__)

#endif /* __LOG_H__ */
