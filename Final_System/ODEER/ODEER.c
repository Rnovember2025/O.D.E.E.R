#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "MI48xx.h"
#include "Test_I2C.h"
#include "Thermal_Camera_Main.h"

#define ROWS 80 
#define COLUMNS 62 
#define TARGET_K 312 
#define TOLERANCE_K 2  

typedef struct {
    uint8_t row;     
    uint8_t column; 
} Coordinate; 

static Coordinate matches[ROWS * COLUMNS]; 

void setup_I2C_0(void) {
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_set_function(17, GPIO_FUNC_I2C);
    gpio_pull_up(16);
    gpio_pull_up(17);
}

uint8_t Write_I2C(uint8_t *data) {

    //uint8_t buffer[1] = {data};
    return i2c_write_timeout_us(i2c0, ESP_32_ADDRESS, data, 2, false, 500);
}

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

int main()
{
    stdio_init_all();

    multicore_launch_core1(thermal_camera_main);

    while (true) {

        //get the pointer to the frame just captured from core 1
        uint32_t frame_ptr = multicore_fifo_pop_blocking();

        printf("Got Frame Pointer: %d\n", frame_ptr);

        size_t match_count = 0; 
        //print the whole frame to the console
        for (int i = 0; i < 62; i++) {
            for (int j = 0; j < 80; j++) {
                uint16_t temp = convertTemp((*frame_ptr)[i][j]);
                printf("%d ", temp);

                //identify hotspots
                uint16_t value = temp[i][j];                 
                uint16_t difference = value > TARGET_K                     
                    ? value - TARGET_K : TARGET_K - value;                  
                if (difference <= TOLERANCE_K) {                     
                    matches[match_count++] = (Coordinate){i, j};                 
                } 
            }
            printf("\n");
        }
        printf("\n\n"); 

        // Write_I2C(uint8_t matches[0]);
        // Write_I2C(uint8_t matches[1]);
        uint8_t coords[] = {30,60};
        Write_I2C(coords);
    }

    while (true) {sleep_sm(1000);}
}