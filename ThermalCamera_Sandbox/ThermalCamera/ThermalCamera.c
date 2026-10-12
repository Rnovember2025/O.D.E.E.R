#include <stdio.h>
#include "MI48xx.h"
#include "pico/time.h"

int main(void)
{
    stdio_init_all();

    Start_I2C();
    Start_SPI();
    Setup_GPIO();

    sleep_ms(2000);
    Reset_Camera();

    printf("Test\n");

    printf("\nScanning I2C Bus...\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        uint8_t rxdata;
        int ret = i2c_read_blocking_until(I2C_PORT, addr, &rxdata, 1, false, make_timeout_time_us(100000));
        if (ret >= 0) {
            printf("I2C device found at address 0x%02X!\n", addr);
        }
    }
    printf("Scan complete.\n");

    // while (true) {
    //     printf("Hello, world!\n");
    //     uint8_t status = I2C_Read(STATUS);
    //     uint8_t frame_mode = I2C_Read(FRAME_MODE);
        
    //     printf("STATUS: 0x%02X | FRAME_MODE: 0x%02X\n", status, frame_mode);
    //     sleep_ms(1000);
    // }
    while (1) {sleep_ms(1000);}
}
