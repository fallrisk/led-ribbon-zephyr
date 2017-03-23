
#include <zephyr.h>

#include <device.h>
#include <spi.h>

void main(void)
{
    // Connect to the SPI device.
    // Pulled from example at https://github.com/zephyrproject-rtos/zephyr/blob/master/samples/drivers/spi_lsm9ds0/src/main.c.
    struct spi_config config = { 0 };
    struct device *spi_mst_0 = device_get_binding("SPI_0");

    if (!spi_mst_0)
        return;

    config.config = SPI_MODEL_CPOL | SPI_MODE_CPHA | SPI_WORD(16);
    config.max_sys_freq = 256;

    int err;
    err = spi_configure(spi_mst_0, &config);
    if (err) {
        printk("Could not configure SPI device.\n");
        return;
    }

    // Send a ribbon packet out the SPI.
    uint8_t rx_buffer[2], tx_buffer[2];
    err = spi_transceive(spi_mst_0, tx_buffer, sizeof(tx_buffer),
        rx_buffer, sizeof(rx_buffer));

    if (err) {
        printk("Error during SPI transfer.\n");
        return;
    }
}
