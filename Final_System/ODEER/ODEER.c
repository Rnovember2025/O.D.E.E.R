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

uint16_t hotspot = 0;
static Coordinate hotspot_location[ROWS * COLUMNS]; 

int main()
{
    stdio_init_all();

    multicore_launch_core1(thermal_camera_main);

    while (true) {

        //get the pointer to the frame just captured from core 1
        uint32_t frame_ptr = multicore_fifo_pop_blocking();

        printf("Got Frame Pointer: %d\n", frame_ptr);

        hotspot = 0; 
        //print the whole frame to the console
        for (int i = 0; i < 62; i++) {
            for (int j = 0; j < 80; j++) {
                uint16_t temp = convertTemp((*frame_ptr)[i][j]);
                printf("%d ", temp);

                //identify hotspot                 
                if (temp > TARGET_K) {
                    if (temp > hotspot){
                        hotspot = temp;
                        hotspot_location = (Coordinate){i, j};
                    } 
                }
            }
            printf("\n");
        }
        printf("\n\n"); 

        // Write_I2C(uint8_t matches[0]);
        // Write_I2C(uint8_t matches[1]);
        uint8_t coords[] = {hotspot_location[0], hotspot_location[1]};
        Write_I2C(coords);
    }

    while (true) {sleep_sm(1000);}
}