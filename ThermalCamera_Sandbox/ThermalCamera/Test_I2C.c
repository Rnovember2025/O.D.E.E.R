#include "Test_I2C.h"
#include "pico/stdlib.h"

void setup_I2C_0(void) {
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_set_function(17, GPIO_FUNC_I2C);
    gpio_pull_up(16);
    gpio_pull_up(17);
}

uint8_t Write_I2C(uint8_t address, uint8_t data) {
    int actual_address;
    if (address >= 0) {
        actual_address = address;
    } else {
        actual_address = ESP_32_ADDRESS_1;
    }

    uint8_t buffer[1] = {data};
    return i2c_write_timeout_us(i2c0, actual_address, buffer, 1, false, 500);
}