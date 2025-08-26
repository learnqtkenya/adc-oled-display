#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "sensors/adc_sensor.h"
#include "display/display_manager.h"

void main(void)
{
    const struct device *adc_dev;
    const struct device *disp_dev;

    int ret = adc_sensor_init(&adc_dev);
    if (ret < 0) return;

    ret = display_manager_init(&disp_dev);
    if (ret < 0) return;

    int16_t raw_value;
    float voltage;

    while (1) {
        ret = adc_sensor_read(adc_dev, &raw_value, &voltage);
        if (ret == 0) {
            printk("ADC Raw: %d, Voltage: %.3f V\n", raw_value, (double)voltage);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "V=%.2f", (double)voltage);
            display_manager_print(disp_dev, buffer, 0, 0);
        }
        k_sleep(K_SECONDS(1));
    }
}
