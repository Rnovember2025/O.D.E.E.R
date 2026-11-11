#include "Thermal_Camera_Main.h"

void thermal_camera_main(void)
{
    sleep_ms(5000);

    Start_SPI();
    Setup_GPIO();
    Start_I2C();

    uint8_t status;
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
        while (millis - last_millis < 1) { //Don't actually need to wait here, but didn't want to completely remove in case it is needed again.
            millis = to_ms_since_boot(get_absolute_time());
        }

        I2C_Write(FRAME_MODE, GET_SINGLE_FRAME | NO_HEADER); //Single frame mode for now, eventually will need to do continuous mode. (if it works).

        //wait for the Data Ready GPIO pin READY_PIN to go high, indicating that the camera has finished capturing a frame
        while (!gpio_get(READY_PIN)) { //could use interupts here eventually.
            sleep_ms(1);
        }

        gpio_put(ON_BOARD_LED, 1);

        SPI_Read(false, frames[frame_number]);

        //send the pointer to the frame just captured
        multicore_fifo_push_blocking((uint32_t) frames[frame_number]);

        gpio_put(ON_BOARD_LED, 0);
        
        last_millis = to_ms_since_boot(get_absolute_time());

        if (++frame_number >= 10) { //Cycles through the ten places to store frames. This hopefully makes it so that the main core doesn't use it at the same time.
            frame_number = 0;
        }
    }
}
