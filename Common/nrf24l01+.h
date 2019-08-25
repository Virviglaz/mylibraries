#ifndef NRF24L01_H
#define NRF24L01_H

#include <stdint.h>
#include <stdbool.h>

struct nrf24l01_conf
{
	/* Interface functions */
	struct
	{
		uint8_t (*write) (uint8_t reg, uint8_t *buf, uint8_t size);
		uint8_t (*read) (uint8_t reg, uint8_t *buf, uint8_t size);
		void (*radio_en) (bool state);
		bool (*read_irq) (void);
	} interface;

	/* Configuration Register */
	struct
	{
		enum { RADIO_TX, RADIO_RX } mode : 1;
		bool power_enable : 1;
		enum { CRC_1B, CRC_2B } crc_config : 1;
		bool crc_enable : 1;

		/* IRQ */
		bool max_rt_irq : 1;
		bool tx_irq : 1;
		bool rx_irq : 1;

		char reserved : 1;
	} config;

	/* Enhanced ShockBurst™ Auto Acknowledgment */
	struct
	{
		bool pipe0 : 1;
		bool pipe1 : 1;
		bool pipe2 : 1;
		bool pipe3 : 1;
		bool pipe4 : 1;
		bool pipe5 : 1;
		char reserved : 2;
	} auto_acknowledgment;

	/* Enabled RX Addresses */
	struct
	{
		bool pipe0 : 1;
		bool pipe1 : 1;
		bool pipe2 : 1;
		bool pipe3 : 1;
		bool pipe4 : 1;
		bool pipe5 : 1;
		char reserved : 2;
	} enabled_rx_addresses;

	/* Setup of Address Widths */
	struct
	{
		enum { 
			ADD_3_BYTES = 1,
			ADD_4_BYTES = 2,
			ADD_5_BYTES = 3,
		} crc_config : 2;
		char reserved : 6;
	}  address_widths;

	/* Setup of Automatic Retransmission */
	struct
	{
		uint8_t count : 4;
		uint8_t delay : 4;
	} auto_retransmit;

	uint8_t channel;

	/* RF Setup Register */
	struct
	{
		enum { /* LNA gain =1 */
			P_18dBm = 0x01,
			P_12dBm = 0x03,
			P_6dBm = 0x05,
			P_0dBm = 0x07,
		} power : 3;
		enum { R_1MPS, R_2MPS } data_rate : 1;
		uint8_t reserved : 4;
	} setup;

	/* Set to NULL if not used */
	uint8_t *rx_address_p0;
	uint8_t *rx_address_p1;
	uint8_t rx_address_p2;
	uint8_t rx_address_p3;
	uint8_t rx_address_p4;
	uint8_t rx_address_p5;
	uint8_t *tx_address;

	/* Size of buffer for each rx pipe */
	uint8_t rx_pipe_size[6];

	/* How many times we gonna pool IRQ before report fault */
	uint32_t read_cnt;

	/* Interface error will be stored in here */
	uint8_t error;

	/* Public functions */
	bool (* send) (uint8_t *data, uint8_t size, bool keep_rx);
	uint8_t (* recv) (uint8_t *data, uint8_t size);
};

struct nrf24l01_conf *nrf24l01_init(struct nrf24l01_conf *driver);

#endif /* NRF24L01_H */