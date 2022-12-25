#include "nvm.h"
#include "nvs_flash.h"
#include "nvs.h"

#define DATABASE_SIZE				0x1000
#define PARTITION_NAME				"database"

void nvm_read(void *dst, size_t size)
{
	const esp_partition_t *p =
		esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
		ESP_PARTITION_SUBTYPE_DATA_NVS, PARTITION_NAME);

	ESP_ERROR_CHECK(!p);
	ESP_ERROR_CHECK(esp_partition_read(p, 0, dst, size));
}

void nvm_write(void *src, size_t size)
{
	const esp_partition_t *p =
		esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
		ESP_PARTITION_SUBTYPE_DATA_NVS, PARTITION_NAME);

	ESP_ERROR_CHECK(!p);
	ESP_ERROR_CHECK(esp_partition_erase_range(p, 0, DATABASE_SIZE));
	ESP_ERROR_CHECK(esp_partition_write(p, 0, src, DATABASE_SIZE));
}
