
#include <zephyr.h>

#include <device.h>
#include <spi.h>
#include <misc/printk.h>

void main(void)
{
	// Connect to the SPI device.
	// Pulled from example at
	// https://github.com/zephyrproject-rtos/zephyr/blob/master/samples/drivers/spi_lsm9ds0/src/main.c.
	struct spi_config config = { 0 };
	struct device *spi_mst_0 = device_get_binding("SPI_0");

	if (!spi_mst_0) {
		return;
	}

	/* Configure the SPI peripheral. */
	config.config = SPI_MODEL_CPOL | SPI_MODE_CPHA | SPI_WORD(8);
	config.max_sys_freq = 250;

	int err;
	err = spi_configure(spi_mst_0, &config);
	if (err) {
		printk("Could not configure SPI device.\n");
		return;
	}

	// Send a byte out SPI.
	uint8_t tx_buffer[1];
	tx_buffer[0] = 'Z';
	err = spi_write(spi_mst_0, tx_buffer, 1);
	if (err) {
		printk("Error during SPI transfer.\n");
		return;
	}

	// 4 + 31 * 3 + 4
	uint8_t ribbon_data[101];
	int i;
	ribbon_data[0] = 0;
	ribbon_data[1] = 0;
	ribbon_data[2] = 0;
	ribbon_data[3] = 0;
	for (i = 0; i < 30; i++) {
		ribbon_data[4 + 3 * i] = 115;
		ribbon_data[4 + 3 * i + 1] = 50;
		ribbon_data[4 + 3 * i + 2] = 100;
	}
	ribbon_data[97] = 0;
	ribbon_data[98] = 0;
	ribbon_data[99] = 0;
	ribbon_data[100] = 0;

	// Send a ribbon packet out the SPI.
	err = spi_write(spi_mst_0, ribbon_data, sizeof(ribbon_data));

	if (err) {
	    printk("Error during SPI transfer.\n");
	    return;
	}
}
