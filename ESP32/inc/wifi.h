#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the Wifi and connect to the network.
 *
 * @param wifi_uuid Access point name.
 * @param wifi_pass Access point password.
 * @param hostname Set the host name of device.
 * @return int null on success, error code if error.
 */
int wifi_init(const char *uuid, const char *pass, const char *hostname);

/**
 * @brief Check wifi is connected.
 *
 * @return true if connected.
 * @return false if not connected.
 */
bool wifi_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */
