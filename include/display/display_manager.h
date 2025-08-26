#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <zephyr/device.h>

/* Initializes the display, sets pixel format, framebuffer */
int display_manager_init(const struct device **disp_dev);

// set font.
int font_adjustment(const struct device *disp_dev);

/* Print text at given coordinates. */
int display_manager_print(const struct device *disp_dev, const char *msg, int x, int y);

#endif // DISPLAY_MANAGER_H
