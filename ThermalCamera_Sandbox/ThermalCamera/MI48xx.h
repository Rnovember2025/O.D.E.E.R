#ifndef MI48XX_H
#define MI48XX_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"

// SPI Defines
#define SPI_PORT spi0
#define SPI_SPEED 1000*1000 //1 MHz
#define PIN_MISO 4
#define PIN_CS   5
#define PIN_SCK  2
#define PIN_MOSI 3
#define REP_TX_DATA 0x0000

// I2C defines
#define I2C_PORT i2c0
#define I2C_SPEED 100*1000 //100 KHz
#define I2C_SDA 6
#define I2C_SCL 7
#define I2C_DEVICE_ADDR 0x40

//GPIO Pin defines
#define READY_PIN 12
#define RESET_PIN 13

//Register addresses on the thermal camera
#define FRAME_MODE 0xB1
#define STATUS     0xB6

//Setting Names
#define GET_SINGLE_FRAME  0b00000001
#define CONTINUOUS_STREAM 0b00000010
#define NO_HEADER         0b00100000

//Function Prototypes
void Start_SPI(void);
void Start_I2C(void);
void Setup_GPIO(void);
void Reset_Camera(void);
void SPI_Read(uint8_t include_header, uint16_t* frame);
void I2C_Write(uint8_t register_addr, uint8_t data);
uint8_t I2C_Read(uint8_t register_addr);

#endif