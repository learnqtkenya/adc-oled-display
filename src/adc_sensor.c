#include "sensors/adc_sensor.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static int16_t adc_sample_buffer[1];

static const struct adc_channel_cfg adc_channel_cfg = {
    .gain             = ADC_GAIN,
    .reference        = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id       = ADC_CHANNEL_ID,
    .differential     = 0,
#ifdef CONFIG_ADC_CONFIGURABLE_INPUTS
    .input_positive   = ADC_CHANNEL_ID,
#endif
};

static struct adc_sequence adc_sequence = {
    .channels    = BIT(ADC_CHANNEL_ID),
    .buffer      = adc_sample_buffer,
    .buffer_size = sizeof(adc_sample_buffer),
    .resolution  = ADC_RESOLUTION,
};

int adc_sensor_init(const struct device **adc_dev)
{
    *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc));
    if (!device_is_ready(*adc_dev)) {
        printk("ERROR: ADC device not ready\n");
        return -ENODEV;
    }

    int ret = adc_channel_setup(*adc_dev, &adc_channel_cfg);
    if (ret < 0) {
        printk("ERROR: ADC channel setup failed (%d)\n", ret);
        return ret;
    }

    printk("ADC initialized on channel %d\n", ADC_CHANNEL_ID);
    return 0;
}

int adc_sensor_read(const struct device *adc_dev, int16_t *raw_value, float *voltage)
{
    int ret = adc_read(adc_dev, &adc_sequence);
    if (ret < 0) {
        printk("ADC read failed (%d)\n", ret);
        return ret;
    }

    *raw_value = adc_sample_buffer[0];
    *voltage = (float)(*raw_value) * 3.3f / ((1 << ADC_RESOLUTION) - 1);

    return 0;
}
