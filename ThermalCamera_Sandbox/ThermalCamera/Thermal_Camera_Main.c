#include <stdio.h>
#include "MI48xx.h"
#include "pico/time.h"
#include "pico/multicore.h"

void thermal_camera_main(void)
{
    sleep_ms(5000);

    Start_SPI();
    Setup_GPIO();
    Start_I2C();

    uint8_t status;
    uint8_t frame_mode;
    uint32_t millis = 0;
    uint32_t last_millis = 0;

    gpio_put(ON_BOARD_LED, 1);
    Reset_Camera();
    //wait for booting up bit to be set
    while(true) {
        status = I2C_Read(STATUS);
        if (status & BOOTING_UP) {break;}
        sleep_ms(50);
    }
    //wait for booting up bit to be cleared
    while(true) {
        status = I2C_Read(STATUS);
        if (!(status & BOOTING_UP)) {break;}
        sleep_ms(50);
    }
    gpio_put(ON_BOARD_LED, 0);

    Enable_Median_Filter();
    
    Frame frames[10];
    int frame_number = 0;

    while (true) {

        millis = to_ms_since_boot(get_absolute_time());
        while (millis - last_millis < 1000) {
            millis = to_ms_since_boot(get_absolute_time());
        }
        
        I2C_Write(FRAME_MODE, GET_SINGLE_FRAME | NO_HEADER);

        //wait for the Data Ready GPIO pin READY_PIN to go high, indicating that the camera has finished capturing a frame
        while (!gpio_get(READY_PIN)) {
            sleep_ms(10);
        }

        gpio_put(ON_BOARD_LED, 1);

        SPI_Read(false, frames[frame_number].pixels);

        //send the pointer to the frame just captured
        multicore_fifo_push_blocking((uintptr_t)&frames[frame_number]);

        gpio_put(ON_BOARD_LED, 0);
        
        last_millis = to_ms_since_boot(get_absolute_time());

        if (++frame_number >= 10) {
            frame_number = 0;
        }
    }
}
