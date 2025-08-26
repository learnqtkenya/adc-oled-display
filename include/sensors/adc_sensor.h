#ifndef ADC_SENSOR_H
#define ADC_SENSOR_H

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

/* ADC configuration */
#define ADC_RESOLUTION      12
#define ADC_GAIN            ADC_GAIN_1
#define ADC_REFERENCE       ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT
#define ADC_CHANNEL_ID      2  /* GPIO 28 -> ADC channel 2 */

int adc_sensor_init(const struct device **adc_dev);
int adc_sensor_read(const struct device *adc_dev, int16_t *raw_value, float *voltage);

#endif // ADC_SENSOR_H
