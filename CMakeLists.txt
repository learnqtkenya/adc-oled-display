cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(adc_oled_display)

# Include directories
target_include_directories(app PRIVATE include)

# Source files
target_sources(app PRIVATE 
    src/main.c
    src/adc_sensor.c
    src/display_manager.c
)
