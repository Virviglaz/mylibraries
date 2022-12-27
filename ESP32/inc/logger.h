#ifndef __LOGGER_H__
#define	__LOGGER_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Default init struct for remote logger.
 *
 * @param x		Server IP address string.
 * @param y		Server port number.
 */
#define LOGGER_DEFAULT_INIT(x, y)	{ \
						.server = (x), \
						.port = (y), \
						.message_queue_size = 1000, \
						.server_poll_timeout = 10000, \
						.printf_buffer_size = 256, \
						.minimum_free_memory = 0x6000, \
						.task_heap_size = 0x1000, \
					}

/**
 * @brief Logger parameters struct.
 */
typedef struct {
	/** Server IP address string. */
	const char *server;

	/** Server port number. */
	uint32_t port;

	/** Message queue size (number of message can wait in the queue)*/
	uint32_t message_queue_size;

	/** How offten check for server availability in ms. */
	uint32_t server_poll_timeout;

	/** Maximum log line in bytes. */
	uint32_t printf_buffer_size;

	/** Minimum free memory of the system before queue will become full. */
	uint32_t minimum_free_memory;

	/** Handler task head size. */
	uint32_t task_heap_size;
} logger_init_t;

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
 * @param[in] logger_init_t	Pointer to init struct.
 */
void message_logger_init(logger_init_t *init);

/**
 * @brief Init message queue and redirect all output to remote server.
 * 
 * @param[in] logger_init_t	Pointer to init struct.
 *
 * @note	This doesn't initialize the handling task. All messages will
 *		be stored in the queue until wifi is connected.
 */
void message_logger_early_init(logger_init_t *init);

#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H__ */
