# Real-time ADC Monitoring with OLED Display: A Practical Zephyr RTOS Project

![adc-oled-display](https://hackmd.io/_uploads/SkdTmu_Kll.jpg)

Embedded systems shine when they bridge the physical and digital worlds. Today's project demonstrates this perfectly by building a real-time ADC monitoring system with OLED display visualization. We'll read analog voltages and display them instantly on a compact screen—a pattern fundamental to countless applications from industrial monitoring to consumer electronics. We'll be using common microcontroller components.

We’ll start by setting up analog voltage readings. You know how these things go—first we configure the input pin for sensor data capture.

Next phase involves getting text onto an **OLED** screen via I²C protocol. Using that **SSD1306** display model everyone’s got in their parts bin these days. The code will need constant updates, grabbing fresh ADC values and converting them into millivolt numbers in real time.

For hardware, grab any RP2040-based board like the **Pi Pico** or our [**SQ_OASIS Node 2**](#Introducing-SQ_OASIS-Node-2:-Professional-Development-Made-Simple) variant works too (see more information about it below). I chose to use RP2040’s ADC system since I had the MCU on my desk. The display’s that standard 128x64 pixel I²C model you see in tutorials all the time. Oh and don’t forget some analog source—potentiometer works fine for testing purposes anyway. For my case, I will use a 10K potentiometer. 

Wiring’s straightforward but double-check those I²C connections people always mix up SDA and SCL pins somehow. Code structure should loop continuously while updating measurements without any fancy delays messing up the refresh rate.

**Connection Diagram**

Connect the OLED display to I2C pins (GPIO 4/5 for SDA/SCL) and your potentiometer to GPIO 28. Refer to the following connection diagram:

<div style="position: relative; width: 100%; padding-top: calc(max(56.25%, 400px));">
  <iframe src="https://app.cirkitdesigner.com/project/982861f2-4ded-4427-b036-727a01f73fd7?view=interactive_preview" style="position: absolute; top: 0; left: 0; width: 100%; height: 100%; border: none;"></iframe>
</div>

---
**Summary of Hardware Components:**

* Raspberry Pi Pico (RP2040) or **SQ_OASIS Node 2** 
* SSD1306 OLED Display
* Potentiometer

### Introducing SQ_OASIS Node 2: Professional Development Made Simple

<div style="background: linear-gradient(135deg,  #2E8B57, #32CD32); padding: 20px; border-radius: 8px; color: white; margin: 25px 0;">
<div style="display: flex; align-items: center; margin-bottom: 15px;">
<div style="width: 50px; height: 50px; background: rgba(255,255,255,0.2); border-radius: 50%; display: flex; align-items: center; justify-content: center; margin-right: 15px; font-size: 24px;">🚀</div>
<div>
<h3 style="margin: 0; color: white; font-size: 1.4em;">Node 2: Next-Generation Development Platform</h3>
<p style="margin: 5px 0 0 0; opacity: 0.9; font-size: 1.1em;">Part of the SQ_OASIS Ecosystem</p>
</div>
</div>

<p style="margin: 0; line-height: 1.6; opacity: 0.95;">While this tutorial works perfectly with standard hardware, we developed it using Node 2 from our SQ_OASIS development ecosystem. Node 2 uses the same RP2040 processor. But we added better connectivity options. The power management feels more robust too. Built-in debugging tools make professional workflows smoother overall.</p>

<div style="margin: 20px 0; padding: 15px; background: rgba(255,255,255,0.1); border-radius: 6px;">
<p style="margin: 0; font-weight: 500;">SQ_OASIS handles connectivity between nodes through CAN, WiFi, Bluetooth, other protocols like that. You could build distributed sensing networks without much hassle. IoT applications work pretty well with this setup too.</p>
</div>

<div style="text-align: center; margin-top: 20px;">
<a href="https://squared.co.ke/services/embedded-systems-design" style="display: inline-block; background: white; color: #FF6B35; padding: 12px 24px; border-radius: 6px; text-decoration: none; font-weight: bold; transition: all 0.3s ease;">Learn More About Our Development Boards →</a>
</div>
</div>

The modular nature of the SQ_OASIS ecosystem means you can start with a single Node 2 for this project and later expand to **multi-node configurations** for more complex applications. This scalability is particularly valuable in professional development where requirements often evolve during the project lifecycle.

We used Zephyr RTOS for demonstration purposes.

---

## Project Structure

```bash
adc-oled-display/
│── boards/
│   └── rpi_pico.overlay
├── include/
│   ├── display/
│   │   └── display_manager.h
│   └── sensors/
│       └── adc_sensor.h
│── src/
│   ├── adc_sensor.c
│   ├── display_manager.c
│   └── main.c
│── CMakeLists.txt
│── prj.conf
└── README.md
```

This structure keeps overlays, headers, and source code organized. You wil find the [source code](https://) on github. I will only reference some sections of configurations file, since that is where the trick lies.

## Zephyr Configuration: Device Trees and Overlays

Zephyr does hardware config through device tree overlays so you don't have to mess with core files. Our setup uses ADC channel 2 on GPIO 28 for sensing stuff. Then there's I2C0 handling the OLED display connections.

You know how overlays work in Zephyr's framework. It's kind of like patching hardware definitions without touching the base code. The ADC part grabs analog signals from pin 28 using that second channel. Meanwhile, I2C0 gets set up to drive the display protocol.

### `boards/rpi_pico.overlay`

```devicetree
/ {
    chosen {
        zephyr,display = &ssd1306;
    };
};

&adc {
    status = "okay";
    channel@2 {
        reg = <2>;
        zephyr,gain = "ADC_GAIN_1";
        zephyr,reference = "ADC_REF_INTERNAL";
        zephyr,resolution = <12>;
    };
};

&i2c0 {
    status = "okay";
    clock-frequency = <I2C_BITRATE_STANDARD>;
    
    ssd1306: ssd1306@3c {
        compatible = "solomon,ssd1306fb";
        reg = <0x3c>;
        height = <64>;
        width = <128>;
        segment-remap;
        com-invdir;
    };
};
```

The project configuration (`prj.conf`) enables ADC, I2C, display drivers, and USB console support:

```
# ADC for current sensing
CONFIG_ADC=y
CONFIG_ADC_SHELL=y

# Display
CONFIG_I2C=y    
CONFIG_DISPLAY=y    
CONFIG_SSD1306=y   
CONFIG_SSD1306_DEFAULT_CONTRAST=128       
CONFIG_LOG=y     
CONFIG_CFB_LOG_LEVEL_DBG=y
CONFIG_CHARACTER_FRAMEBUFFER=y
CONFIG_GPIO=y
CONFIG_PINCTRL=y

# Floating point support for printf 
CONFIG_CBPRINTF_FP_SUPPORT=y

# Enable heap memory (needed for k_malloc / k_free)  
CONFIG_HEAP_MEM_POOL_SIZE=16384    
 
```

**ADC Channel Configuration Deep Dive:**
The `channel@2` in `boards/rpi_pico.overlay` configures ADC channel 2 with specific parameters critical for accurate voltage measurement:
- `zephyr,gain = "ADC_GAIN_1"` sets unity gain, meaning input voltage equals measured voltage
- `zephyr,reference = "ADC_REF_INTERNAL"` uses the RP2040's internal 3.3V reference for consistency
- `zephyr,resolution = <12>` enables full 12-bit resolution (4096 discrete levels)

**I2C and SSD1306 Configuration:**
The SSD1306 configuration includes display-specific parameters that directly affect operation:
- `segment-remap` flips the display horizontally - crucial for proper text orientation
- `com-invdir` inverts the COM scan direction for correct vertical orientation  
- `multiplex-ratio = <63>` sets the display for 64-row operation (0-63)
- The I2C address `0x3c` is standard for most SSD1306 modules, but some use `0x3d`

## Looking closer at ADC setup for the RP2040

Turns out that subsystem has more going on than just checking voltages if you dig into it. People don't always realize that at first glance. Getting familiar with what's under the hood really helps when building sensor setups that need better reliability or precision, you know. Not just plug-and-play stuff here.

### ADC Architecture and Capabilities

So the RP2040 has multiple 12-bit ADC input channels. Looking at it, I found that it can handle up to 500k samples a second. But most setups run it way slower for stability reasons.

The internal voltage reference works without needing extra components. You know how portable devices hate complicated wiring. Accuracy stays solid though. Makes sense for battery-powered stuff people carry around.

Built-in sensors simplify things compared to external modules. No messy calibration routines either. Just works out of the box most times. Hardware guys seem to dig that simplicity.

Key technical considerations for our implementation:
- **Acquisition Time**: The `ADC_ACQ_TIME_DEFAULT` setting gives enough time for the input capacitor to charge up fully. You know how that’s needed for getting accurate measurements right.
- **Input Impedance**: The ADC input impedance sits around 100kΩ usually, which works fine for most sensor setups out there. But here’s the thing – if you’re dealing with high-impedance sources, you might want to throw in a buffer circuit just to be safe.
- **Noise Performance**: The 12-bit resolution provides theoretical noise floor of ~0.8mV, though practical noise is typically 2-3 LSBs

### ADC Sensor Module Implementation

The implementation uses three modules: ADC sensor, display manager, and main application logic.

### `include/sensors/adc_sensor.h`

```c
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

```

**Critical Implementation Details:**
- The ADC sequence structure defines exactly how the measurement occurs
- Buffer size must match the expected data type (int16_t for 12-bit results)
- The voltage conversion uses the exact reference voltage for calibrated results
- Error checking ensures reliable operation even with hardware failures

## SSD1306 OLED Driver Deep Dive

The SSD1306 OLED driver handles controlling those 128x64 pixel screens using I2C signals. Knowing how its guts work helps squeeze better performance out of your display and fix common headaches when they pop up.

### SSD1306 Architecture and Communication

SSD1306 has got this built-in graphics memory called GDRAM holding the whole screen image in its brain. The chip keeps refreshing the OLED from there nonstop, which means no annoying flicker while stuff changes on screen. Kind of like how those Instagram stories you flip through stay smooth even when updating.

That memory setup lets you push new visuals without making things jump around visually. You can tweak parts of the screen while other areas stay rock solid. Pretty neat system when you think about it.

Key technical aspects of SSD1306 operation:
- **Page-based Memory**: Display memory is organized in 8 pages of 128 bytes each
- **I2C Protocol**: Uses standard I2C with command/data selection via control byte
- **Power Management**: Built-in charge pump eliminates need for external high voltage

### Display Manager Implementation

### `include/display/display_manager.h`

```c
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <zephyr/device.h>

/* Initializes the display, sets pixel format, framebuffer, and default font. */
int display_manager_init(const struct device **disp_dev);

/* Change font by index (from cfb fonts). */
int display_manager_set_font(const struct device *disp_dev, int font_idx);

/* Print text at given coordinates. */
int display_manager_print(const struct device *disp_dev, const char *msg, int x, int y);

#endif // DISPLAY_MANAGER_H

```

**Key SSD1306 Implementation Points:**
- `cfb_framebuffer_init()` configures the display controller and loads fonts into memory
- `cfb_framebuffer_clear(false)` clears local framebuffer without I2C transfer
- `cfb_print()` renders text into the framebuffer using bitmap fonts
- `cfb_framebuffer_finalize()` performs the actual I2C transfer to update the display
- This separation allows multiple text operations before a single display update, reducing I2C traffic

### I2C Communication Optimization

The display update process involves transferring 1024 bytes (128x64 pixels ÷ 8 bits/byte) via I2C. Zephyr's CFB (Character Frame Buffer) system optimizes this by:
- Batching multiple text operations into a single transfer
- Using I2C burst writes to minimize protocol overhead
- Maintaining a local framebuffer to avoid unnecessary display reads

For applications requiring frequent updates, consider implementing dirty region tracking to update only changed portions of the display.

## Integration: Bringing ADC and Display Together

The main application demonstrates proper integration of both subsystems while handling error conditions and timing requirements.

### Main Application Logic

```c
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
```

**System Integration Considerations:**
- **Error Handling**: Both ADC and display operations can fail; proper error handling prevents system lockup
- **Resource Management**: Zephyr automatically handles I2C arbitration if multiple devices share the bus
- **Power Consumption**: The sleep interval allows the system to enter low-power states between measurements

This implementation showcases professional embedded development practices with robust error handling, clean module separation, and efficient resource utilization. The modular design makes it easy to add features like data logging, multiple sensors, or remote connectivity while maintaining system reliability.

## Building and Flashing the Freestanding Zephyr Application

If you’ve already set up Zephyr on your Ubuntu machine by following the [official getting started guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html), building and running this project is straightforward.

### 1. Activate the Zephyr Environment

Navigate to your project folder and activate the Python virtual environment and Zephyr environment:

```bash
cd path/to/adc-oled-display
source ~/zephyrproject/.venv/bin/activate
source ~/zephyrproject/zephyr/zephyr-env.sh
```

### 2. Build the Application

Run the following command to build for the Raspberry Pi Pico (`rpi_pico` board):

```bash
west build -b rpi_pico . -p always
```

If successful, the build system will generate a UF2 binary in the `build/zephyr/` directory.

### 3. Flash to the Board

To flash, put your Raspberry Pi Pico into **bootloader mode** by holding the **BOOTSEL** button while plugging it into your PC. Then run:

```bash
west flash --runner uf2
```

Zephyr will copy the firmware to the Pico, and the board will reboot automatically.

**Note:**

If your board comes with a **BOOTSEL** and a **RESET** button, you might find it easier to hold down the BOOTSEL button and then press RESET, all while the board is still plugged in via USB. This will trigger the boot mode without having to constantly unplug and plug in the USB cable.

The board should be detected as USB Mass Storage Device, which you can mount in your file explorer just like a USB drive.

Then copy ***.uf2** onto the device. It should reboot itself into Zephyr once the UF2 file is done copying over!

### 4. Testing: Expected Output

* The **ADC values** sampled from GPIO 28 (ADC channel 2) will be displayed on the **OLED screen**.
* At the same time, the raw ADC values and voltage readings will be printed to the **USB console**.

![adc-oled-display-2](https://hackmd.io/_uploads/B1NdE__Fxe.jpg)

## Customization Ideas

The modular architecture supports easy enhancements. Add multiple ADC channels for multi-sensor monitoring, implement data logging to SD card, or add WiFi connectivity for remote monitoring. Displays here can handle graphs. Trend indicators too. Alarm conditions pop up when needed. They've got extra graphics capabilities for those features

If you're using this in factories, you might want CAN bus comms. Modbus integration is another option. Same basic setup works for bigger monitoring systems too.



*Ready to implement embedded monitoring in your products? Our [firmware development](https://squared.co.ke/services/embedded-firmware) team specializes in turning concepts into commercial solutions.*

<div style="background: linear-gradient(135deg, #2E8B57, #32CD32); padding: 15px; border-radius: 6px; color: white; margin: 20px 0; text-align: center;">
<p style="margin: 0; font-size: 1.1em; font-weight: 500;">Need help bringing your embedded project ideas to life? From prototype to production, our development services team delivers results.</p>
</div>
