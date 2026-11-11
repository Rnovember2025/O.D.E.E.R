#ifndef Test_I2C_h
#define Test_I2C_h

#include "hardware/i2c.h"

#define ESP_32_ADDRESS_1 0x55 //placeholder value
#define ESP_32_ADDRESS_2 0xD5

void setup_I2C_0(void);
uint8_t Write_I2C(uint8_t address, uint8_t data); //Returns number of bytes written, or PICO_ERROR_GENERIC if failed.

#endif