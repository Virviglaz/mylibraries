#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize the Wifi and connect to the network.
 *
 * @param[in]		Wifi_uuid Access point name.
 * @param[in]		Wifi_pass Access point password.
 * @return int		0 on success, error code on error.
 */
int wifi_init(const char *uuid, const char *pass);

/**
 * @brief Connect to server.
 * 
 * @param[out] socketfd	Pointer to socket file descriptor.
 * @param[in] server	Pointer to server IP address string.
 * @param[in] port	Server port number.
 *
 * @return int		0 on success, error code on error.
 */
int connect_to_server(int *socketfd, const char *server, uint32_t port);

/**
 * @brief Check wifi is connected.
 *
 * @return		true if connected.
 * @return		false if not connected.
 */
bool wifi_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */
