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

	/** Check new firmware interval in ms (5000 ms default)*/
	uint32_t check_interval_ms;

	/** Ota message payload size in bytes (1000 default) */
	uint32_t message_size;

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

/**
 * @brief Typical partitions.csv file for 4MB SPI data flash
 *
# Name,   Type, SubType,  Offset,   Size,  Flags
nvs,      data, nvs,      0x9000,  0x4000
otadata,  data, ota,      0xd000,  0x2000
phy_init, data, phy,      0xf000,  0x1000
factory,  app,  factory,  0x10000, 0x14E000
ota_0,    app,  ota_0,    ,        0x14E000
ota_1,    app,  ota_1,    ,        0x14E000
nvs_key,  data, nvs_keys, ,        0x1000
database, data, nvs,      ,        0x1000
*/

/**
 * @brief To fix GPIO12 high causing flash write error use:
 *
 * espefuse.py --port COM16 set_flash_voltage 3.3V
 */

#endif /* __OTA_H__ */
