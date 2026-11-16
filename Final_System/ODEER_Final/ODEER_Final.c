#include <stdio.h>
#include "pico/stdlib.h"
#include "HotspotDetection.h"
#include "Thermal_Camera_Main.h"
#include "MI48xx.h"
#include "I2C_To_ESP32.h"


int main()
{
    stdio_init_all();

    setup_I2C_0();

    multicore_launch_core1(thermal_camera_main);

    Coordinate hotspot_location;
    hotspot_location.row = 0;
    hotspot_location.column = 0;
    
    uint8_t success = 0;

    while (true) {
        Frame *frame_ptr = (Frame *)multicore_fifo_pop_blocking();

        printf("Got Frame Pointer: %d\n", (int)frame_ptr);

        gpio_put(ON_BOARD_LED, 1);

        Detect_Hotspot(frame_ptr, &hotspot_location);

        success = Write_ESP32(&hotspot_location);

        printf("Hotspot Location: Row: %d, Column: %d\n\n", hotspot_location.row, hotspot_location.column);

        gpio_put(ON_BOARD_LED, 0);
    }
}
