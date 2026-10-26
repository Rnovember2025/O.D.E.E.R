#include <stdio.h>
#include "MI48xx.h"
#include "pico/time.h"

uint16_t convertTemp(uint16_t hexValue) {
    return ((hexValue / 10) - 273.15) * 1.8 + 32; // Convert to Fahrenheit
}

int main(void)
{
    stdio_init_all();

    sleep_ms(5000);

    Start_SPI();
    Setup_GPIO();
    Start_I2C();

    uint8_t status;
    uint8_t frame_mode;

    Reset_Camera();
    //wait for booting up bit to be set
    while(true) {
        status = I2C_Read(STATUS);
        printf("STATUS: 0x%02X\n", status);
        if (status & BOOTING_UP) {break;}
        sleep_ms(50);
    }

    //wait for booting up bit to be cleared
    while(true) {
        status = I2C_Read(STATUS);
        printf("STATUS: 0x%02X\n", status);
        if (!(status & BOOTING_UP)) {break;}
        sleep_ms(50);
    }

    Enable_Median_Filter();

    uint16_t frame[62][80] = {0};

    while (true) {
        //printf("Hello, world!\n");
        
        I2C_Write(FRAME_MODE, GET_SINGLE_FRAME | NO_HEADER);

        frame_mode = I2C_Read(FRAME_MODE);
        status = I2C_Read(STATUS);
        printf("STATUS: 0x%02X | FRAME_MODE: 0x%02X\n", status, frame_mode);
        while (!(status & DATA_READY)) {
            sleep_ms(10);
            frame_mode = I2C_Read(FRAME_MODE);
            status = I2C_Read(STATUS);
            printf("STATUS: 0x%02X | FRAME_MODE: 0x%02X\n", status, frame_mode);
        }

        SPI_Read(false, frame);

        for (int i = 0; i < 62; i++) {
            for (int j = 0; j < 80; j++) {
                uint16_t temp = convertTemp(frame[i][j]);
                //print raw hex values only
                //printf("%04X ", frame[i][j]);

                //print converted temperature values
                printf("%3d ", temp);
            }
            printf("\n");
        }
        printf("\n\n");

    }
    while (1) {sleep_ms(1000);}
}
