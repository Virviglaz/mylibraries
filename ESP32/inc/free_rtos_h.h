#ifndef __FREERTOS_H_H__
#define __FREERTOS_H_H__

/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

/* Helpers */
#define delay(x)			vTaskDelay(pdMS_TO_TICKS(x))
#define delay_ms(x)			delay(x)
#define delay_s(x)			delay_ms(x * 1000)

#define task_create(c, n, s, p, pr, h)	xTaskCreate(c, n, s, p, pr, h)
#define task_delete(t)			vTaskDelete(t)

#define queue_create(l, s) 		xQueueCreate(l, s)
#define queue_send(q, i, t)		xQueueSend(q, i, pdMS_TO_TICKS(t))
#define queue_receive(q, b, t)		xQueueReceive(q, b, pdMS_TO_TICKS(t))

#define semaphore_create_binary()	xSemaphoreCreateBinary()
#define semaphore_create_mutex()	xSemaphoreCreateMutex()
#define semaphore_take(s,t) 		xSemaphoreTake(s, pdMS_TO_TICKS(t))
#define semaphore_give(s)		xSemaphoreGive(s)
#define semaphore_delete(s)		vSemaphoreDelete(s)

#endif /* __FREERTOS_H_H__ */
