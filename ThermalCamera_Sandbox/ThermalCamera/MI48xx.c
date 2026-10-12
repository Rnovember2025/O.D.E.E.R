#include "MI48xx.h"

void Start_SPI(void) {
    // SPI initialisation. 
    spi_init(SPI_PORT, SPI_SPEED);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    //The MI48xx uses 16 bit words and Mode 0.
    spi_set_format(SPI_PORT, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi
}

void Start_I2C(void) {
    // I2C Initialisation. 
    i2c_init(I2C_PORT, I2C_SPEED);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c
}

void Setup_GPIO(void) {
    gpio_init(READY_PIN);
    gpio_set_dir(READY_PIN, GPIO_IN);

    gpio_init(RESET_PIN);
    gpio_set_dir(RESET_PIN, GPIO_OUT);
    gpio_put(RESET_PIN, 1);
}

void Reset_Camera(void) {
    gpio_put(RESET_PIN, 0);
    sleep_ms(1);
    gpio_put(RESET_PIN, 1);
    sleep_ms(500);
}

void SPI_Read(uint8_t include_header, uint16_t* frame) {
    //If you include the header, you must allocate 5040 words for the frame.
    uint16_t length;
    uint8_t settings = I2C_Read(FRAME_MODE);
    if (include_header) {
        settings = settings & ~NO_HEADER;
        length = 5040;
    } else {
        settings = settings | NO_HEADER;
        length = 4960;
    }
    I2C_Write(FRAME_MODE, settings);

    gpio_put(PIN_CS, 0);
    spi_read16_blocking(SPI_PORT, REP_TX_DATA, frame, length);
    gpio_put(PIN_CS, 1);
}

void I2C_Write(uint8_t register_addr, uint8_t data) {
    const uint8_t data_stream[] = {register_addr, data};
    i2c_write_blocking(I2C_PORT, I2C_DEVICE_ADDR, data_stream, 2, false);
}

uint8_t I2C_Read(uint8_t register_addr) {
    uint8_t data = 0;
    i2c_write_blocking (I2C_PORT, I2C_DEVICE_ADDR, &register_addr, 1, true);
    i2c_read_blocking (I2C_PORT, I2C_DEVICE_ADDR, &data, 1, false);
    return data;
}