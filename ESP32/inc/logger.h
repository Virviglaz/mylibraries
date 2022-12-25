#ifndef __LOGGER_H__
#define	__LOGGER_H__

#include "esp_log.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Switch the message pool destanation: local UART output or remote server.
 *
 * @param[in] False for uart output and true for remote server.
 */
void switch_printf(bool remote);

/**
 * Initialize message queue and start handling task for message pool.
 *
 * @param[in] server	Pointer to server IP address string.
 * @param[in] port	Server port number.
 */
void message_logger_init(const char *server, uint32_t port);

/**
 * @brief Init message queue and redirect all output to remote server.
 * 
 * @param[in] server	Pointer to server IP address string.
 * @param[in] port	Server port number.
 *
 * @note	This doesn't initialize the handling task. All messages will
 *		be stored in the queue until wifi is connected.
 */
void message_logger_early_init(const char *server, uint32_t port);

#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H__ */
