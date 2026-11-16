#ifndef I2C_To_ESP32_h
#define I2C_To_ESP32_h
#include "hardware/i2c.h"
#include "HotspotDetection.h"

#define ESP_32_ADDRESS 0x55

void setup_I2C_0(void);
uint8_t Write_ESP32(Coordinate *coords); //Returns number of bytes written, or PICO_ERROR_GENERIC if failed.

#endif