#ifndef __OTA_H__
#define __OTA_H__

#include <stdint.h>

/**
 * @brief OTA initialization settings struct.
 */
typedef struct {
	/** OTA server IP address */
	const char *server_ip;

	/** OTA server port */
	uint32_t server_port;

	/** Device serial number for logging purpose */
	uint32_t serial_number;

	/** Check new firmware interval in ms */
	uint32_t check_interval_ms;

	/** Magic word to recognize specific application */
	uint32_t uniq_magic_word;

	/** Actual version of application, NULL to get automatically from IDF */
	const char *version;

	/** GPIO workaround callback if required */
	void (*gpio_ota_workaround)(void);

	/** GPIO workaround cancellation callback */
	void (*gpio_ota_cancel_workaround)(void);
} ota_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start OTA client.
 *
 * @param settings Pointer to OTA settings.
 * @return int 0 if success, error code if fail.
 */
int ota_start(ota_t *settings);

/**
 * @brief Confirm that actual firmware is valid.
 *
 * @return int 0 if success, error code if fail.
 */
int ota_confirm(void);

/**
 * @brief Stop OTA client.
 *
 * @return int 0 if success, error code if fail.
 */
int ota_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __OTA_H__ */
