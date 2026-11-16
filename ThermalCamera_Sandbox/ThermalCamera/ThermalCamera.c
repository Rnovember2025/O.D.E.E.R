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

    while (true) {

        uint8_t data[20];
        i2c_read_blocking(i2c0, ESP_32_ADDRESS, data, 5, false);
        printf("I2C Read Data: %d\n", data);

        // sleep_ms(1000);

        // int success;

        // // success = Write_I2C(0x68, 0x41, 0x4C, 0x4C, 0x4F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x00);
        // uint8_t data[] = {0x68, 0x65, 0x6C, 0x6C, 0x6F};
        // success = Write_I2C(data, sizeof(data));
        // printf("I2C Write Success: %d\n", success);

        sleep_ms(1000);
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