#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libbladeRF.h>

/* The RX and TX modules are configured independently for these parameters */
struct module_config {
    bladerf_module module;

    unsigned int frequency;
    unsigned int bandwidth;
    unsigned int samplerate;

    /* Gains */
    bladerf_lna_gain rxlna;
    int vga1;
    int vga2;
};

int configure_module(struct bladerf *dev, struct module_config *c)
{
    int status;

    status = bladerf_set_frequency(dev, c->module, c->frequency);
    if (status != 0) {
        fprintf(stderr, "Failed to set frequency = %u: %s\n",
                c->frequency, bladerf_strerror(status));
        return status;
    }

    status = bladerf_set_sample_rate(dev, c->module, c->samplerate, NULL);
    if (status != 0) {
        fprintf(stderr, "Failed to set samplerate = %u: %s\n",
                c->samplerate, bladerf_strerror(status));
        return status;
    }

    return status;
}

/* Usage:
 *   libbladeRF_example_boilerplate [serial #]
 *
 * If a serial number is supplied, the program will attempt to open the
 * device with the provided serial number.
 *
 * Otherwise, the first available device will be used.
 */
int main(int argc, char *argv[])
{
    int status;
    struct bladerf *dev = NULL;
    struct bladerf_devinfo dev_info;
    struct module_config config;

    /* Initialize the information used to identify the desired device
     * to all wildcard (i.e., "any device") values */
    bladerf_init_devinfo(&dev_info);

    /* Request a device with the provided serial number.
     * Invalid strings should simply fail to match a device. */
    if (argc >= 2) {
        strncpy(dev_info.serial, argv[1], sizeof(dev_info.serial) - 1);
    }

    status = bladerf_open_with_devinfo(&dev, &dev_info);
    if (status != 0) {
        fprintf(stderr, "Unable to open device: %s\n",
                bladerf_strerror(status));

        return 1;
    }

    /* Set up RX module parameters */
    config.module     = BLADERF_MODULE_RX;
    config.frequency  = 910000000;
    config.bandwidth  = 2000000;
    config.samplerate = 300000;
    config.rxlna      = BLADERF_LNA_GAIN_MAX;
    config.vga1       = 30;
    config.vga2       = 3;

    status = configure_module(dev, &config);
    if (status != 0) {
        fprintf(stderr, "Failed to configure RX module. Exiting.\n");
        goto out;
    }

    /* Set up TX module parameters */
    config.module     = BLADERF_MODULE_TX;
    config.frequency  = 918000000;
    config.bandwidth  = 1500000;
    config.samplerate = 250000;
    config.vga1       = -14;
    config.vga2       = 0;

    status = configure_module(dev, &config);
    if (status != 0) {
        fprintf(stderr, "Failed to configure TX module. Exiting.\n");
        goto out;
    }

    /* Application code goes here.
     *
     * Don't forget to call bladerf_enable_module() before attempting to
     * transmit or receive samples!
     */

out:
    bladerf_close(dev);
    return status;
}
