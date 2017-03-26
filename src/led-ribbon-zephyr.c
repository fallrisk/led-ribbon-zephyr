
#include <zephyr.h>

#include <kernel.h>
#include <device.h>
#include <spi.h>
#include <misc/printk.h>

int toggle;

void SendRibbonData(struct device *spi_mst_0)
{
	int err;

	// 4 + 31 * 3 + 4
	uint16_t ribbon_data[101];
	int i;
	ribbon_data[0] = 0;
	ribbon_data[1] = 0;
	ribbon_data[2] = 0;
	ribbon_data[3] = 0;
	for (i = 0; i < 31; i++) {
		/* G, R, B */
		if (toggle) {
			ribbon_data[4 + 3 * i] = 20 | 0x80;
			ribbon_data[4 + 3 * i + 1] = 50 | 0x80;
			ribbon_data[4 + 3 * i + 2] = 60 | 0x80;
		} else {
			ribbon_data[4 + 3 * i] = 90 | 0x80;
			ribbon_data[4 + 3 * i + 1] = 100 | 0x80;
			ribbon_data[4 + 3 * i + 2] = 10 | 0x80;
		}
	}
	ribbon_data[97] = 0;
	ribbon_data[98] = 0;
	ribbon_data[99] = 0;
	ribbon_data[100] = 0;

	// Send a ribbon packet out the SPI.
	err = spi_write(spi_mst_0, ribbon_data, 101);

	if (err) {
		printk("Error during SPI transfer.\n");
		return;
	}

	if (toggle) {
		toggle = 0;
	} else {
		toggle = 1;
	}
}

void main(void)
{
	toggle = 0;

	// Connect to the SPI device.
	// Pulled from example at
	// https://github.com/zephyrproject-rtos/zephyr/blob/master/samples/drivers/spi_lsm9ds0/src/main.c.
	struct spi_config config = { 0 };
	struct device *spi_mst_0 = device_get_binding("SPI_0");

	if (!spi_mst_0) {
		return;
	}

	/* Configure the SPI peripheral. */
	config.config = SPI_WORD(8);
	config.max_sys_freq = 150;

	int err;
	err = spi_configure(spi_mst_0, &config);
	if (err) {
		printk("Could not configure SPI device.\n");
		return;
	}

	while (1) {
		SendRibbonData(spi_mst_0);
		k_sleep(1000);
	}
}
