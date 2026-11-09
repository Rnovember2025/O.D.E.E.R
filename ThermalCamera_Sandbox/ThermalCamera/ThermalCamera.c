#include <stdio.h>
#include "MI48xx.h"
#include "Thermal_Camera_Main.h"
#include "pico/multicore.h"


int main() {
    stdio_init_all();

    sleep_ms(2000);
 
    multicore_launch_core1(thermal_camera_main);

    while (true) {
        //get the pointer to the frame just captured from core 1
        Frame *frame_ptr = (Frame *) multicore_fifo_pop_blocking();

        //print the whole frame to the console
        for (int i = 0; i < 62; i++) {
            for (int j = 0; j < 80; j++) {
                uint16_t temp = convertTemp(frame_ptr->pixels[i][j]);
                printf("%d ", temp);
            }
            printf("\n");
        }
        printf("\n\n");

    }
    
    while (1) {sleep_ms(1000);}
}