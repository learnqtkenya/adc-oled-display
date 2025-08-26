#include "display/display_manager.h"
#include <zephyr/display/cfb.h>

static const struct device *display_dev;

int display_manager_init(const struct device **disp_dev)
{
    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        return -ENODEV;
    }

    // Initialize the character framebuffer - this sets up font rendering
    if (cfb_framebuffer_init(display_dev) != 0) {
        return -EIO;
    }

    // call the font function
    font_adjustment(display_dev);

    // Clear display and update - this sends initialization commands to SSD1306
    cfb_framebuffer_clear(display_dev, true);
    
    *disp_dev = display_dev;
    return 0;
}

int font_adjustment(const struct device *disp_dev) {    
    uint16_t x_res;
    uint16_t y_res;
    uint16_t rows;
    uint8_t ppt;
    uint8_t font_width;
    uint8_t font_height;
    
    // Let's set the screen resolution
    x_res = cfb_get_display_parameter(disp_dev, CFB_DISPLAY_WIDTH);
    y_res = cfb_get_display_parameter(disp_dev, CFB_DISPLAY_HEIGHT);
    rows = cfb_get_display_parameter(disp_dev, CFB_DISPLAY_ROWS);
    ppt = cfb_get_display_parameter(disp_dev, CFB_DISPLAY_PPT);

    // now we can set the font
    for (int idx = 0; idx < 42; idx++) {
        if (cfb_get_font_size(disp_dev, idx, &font_width, &font_height)) {
            break;
        }
        cfb_framebuffer_set_font(disp_dev, idx);
    }
    return 0;
}
int display_manager_print(const struct device *disp_dev, const char *msg, int x, int y)
{
    // Clear the framebuffer but don't update display yet
    cfb_framebuffer_clear(disp_dev, false);
    
    // Print text to framebuffer at specified coordinates
    if (cfb_print(disp_dev, msg, x, y) != 0) {
        return -EIO;
    }
    
    // Transfer framebuffer to display GDRAM via I2C
    if (cfb_framebuffer_finalize(disp_dev) != 0) {
        return -EIO;
    }
    
    return 0;
}
