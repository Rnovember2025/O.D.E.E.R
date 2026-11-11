#include <stdio.h>
#include "MI48xx.h"
#include "Test_I2C.h"
#include "Thermal_Camera_Main.h"
#include "pico/multicore.h"

int main() {
    stdio_init_all();

    sleep_ms(2000);

    setup_I2C_0();
 
    //multicore_launch_core1(thermal_camera_main);

    for (int i = 0; i <= 0xFF; i++) {
        uint8_t result = Write_I2C(0x00);
        printf("Result: %d\n", result);
        if (result == PICO_ERROR_GENERIC) {
            printf("I2C Write to address 0x%02X failed.\n\n", i);
        } else {
            printf("I2C Write to address 0x%02X succeeded.\n\n", i);
        }
    }

    while (true) {


        //get the pointer to the frame just captured from core 1
        //uint32_t frame_ptr = multicore_fifo_pop_blocking();

        //printf("Got Frame Pointer: %d\n", frame_ptr);

        // //print the whole frame to the console
        // for (int i = 0; i < 62; i++) {
        //     for (int j = 0; j < 80; j++) {
        //         uint16_t temp = convertTemp((*frame_ptr)[i][j]);
        //         printf("%d ", temp);
        //     }
        //     printf("\n");
        // }
        // printf("\n\n");

    }
    
    while (1) {sleep_ms(1000);}
}