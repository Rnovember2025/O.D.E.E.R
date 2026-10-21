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
    sleep_ms(10);
    gpio_put(RESET_PIN, 1);
    sleep_ms(10);
}

void SPI_Read(uint8_t include_header, uint16_t frame[62][80]) {
    //read the thermal camera data over SPI. The pointer passed is a two dimensional array of 80x62 16-bit values. 
    //The include_header parameter is a boolean that indicates whether to include the 16-bit header in the data read.
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

    uint16_t temp_frame[length];

    gpio_put(PIN_CS, 0);
    spi_read16_blocking(SPI_PORT, REP_TX_DATA, temp_frame, length);
    gpio_put(PIN_CS, 1);

    //reorganize the data into a 2D array of 80x62 values
    for (int i = 0; i < 62; i++) {
        for (int j = 0; j < 80; j++) {
            frame[i][j] = temp_frame[i * 80 + j];
        }
    }

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



uint8_t Get_Status(void) {
    //Bit 1: READOUT_TO_SLOW
    //Bit 2: SENXOR_IF_ERROR
    //Bit 3: CAPTURE_ERROR
    //Bit 4: DATA_READY
    //Bit 5: BOOTING_UP
    //All bits active high.
    return I2C_Read(STATUS) & 0b00111110;
}



void Switch_Access_To_Flash(void) {
    I2C_Write(USER_FLASH_CTRL, USER_FLASH_ENABLE);
}

void Switch_Access_To_Normal(void) {
    I2C_Write(USER_FLASH_CTRL, ~USER_FLASH_ENABLE);
}



//A temporal filter smooths out the camera data over time, providing a cleaner image, 
// but if the filtering is too high, it can cause the image to smear.
void Enable_Temporal_Filter(void) {
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    I2C_Write(FILTER_CONTROL, filter_control | TEMPORAL_ENABLE | TEMPORAL_INITIATE);
}

void Disable_Temporal_Filter(void) {
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    I2C_Write(FILTER_CONTROL, filter_control & ~TEMPORAL_ENABLE | TEMPORAL_INITIATE);
}

void Set_Temporal_Filter(uint16_t filter_strength) {
    uint8_t filter_setting_1 = filter_strength & 0b0000000011111111;
    uint8_t filter_setting_2 = filter_strength >> 8;
    I2C_Write(FILTER_SETTING_1, filter_setting_1);
    I2C_Write(FILTER_SETTING_1 + 1, filter_setting_2);
}

uint16_t Get_Temporal_Filter(void) {
    uint8_t filter_setting_1 = I2C_Read(FILTER_SETTING_1);
    uint8_t filter_setting_2 = I2C_Read(FILTER_SETTING_1 + 1);
    return (filter_setting_2 << 8) | filter_setting_1;
}



//A rolling average filter smooths out fast changing data, while keeping long term trends.
void Enable_Rolling_Average_Filter(void) {
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    I2C_Write(FILTER_CONTROL, filter_control | ROLL_AVG_ENABLE | TEMPORAL_INITIATE);
}

void Disable_Rolling_Average_Filter(void) {
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    I2C_Write(FILTER_CONTROL, filter_control & ~ROLL_AVG_ENABLE | TEMPORAL_INITIATE);
}

void Set_Rolling_Average_Filter(uint8_t filter_strength) {
    I2C_Write(FILTER_SETTING_2, filter_strength);
}

uint8_t Get_Rolling_Average_Filter(void) {
    return I2C_Read(FILTER_SETTING_2);
}



//A median filter can reduce noise while keeping the image details sharp.
//  It replaces a pixel with the median value of its neighbors.
void Enable_Median_Filter(void) {
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    I2C_Write(FILTER_CONTROL, filter_control | MEDIAN_ENABLE | TEMPORAL_INITIATE);
}

void Disable_Median_Filter(void) {
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    I2C_Write(FILTER_CONTROL, filter_control & ~MEDIAN_ENABLE | TEMPORAL_INITIATE);
}

void Set_Median_Kernel(uint8_t kernel_size) {
    //boolean kernal_size selects either a 3x3 or 5x5 median filter. 0 = 3x3; 1 = 5x5
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    if (kernel_size) {
        filter_control = filter_control | MEDIAN_KERNEL_SELECT;
    } else {
        filter_control = filter_control & ~MEDIAN_KERNEL_SELECT;
    }
    I2C_Write(FILTER_CONTROL, filter_control | TEMPORAL_INITIATE);
}

uint8_t Get_Median_Kernel(void) {
    uint8_t filter_control = I2C_Read(FILTER_CONTROL);
    return filter_control & MEDIAN_KERNEL_SELECT;
}



void Set_Frame_Rate_Divider(uint8_t frame_rate) {
    //A value of zero sets the frame rate to maximum.
    I2C_Write(FRAME_RATE, frame_rate & 0b01111111);
}

uint8_t Get_Frame_Rate_Divider(void) {
    return I2C_Read(FRAME_RATE) & 0b01111111;
}



void Set_Frame_Mode(uint8_t frame_mode) {
    I2C_Write(FRAME_MODE, frame_mode & (GET_SINGLE_FRAME | NO_HEADER | CONTINUOUS_STREAM));
}

uint8_t Get_Frame_Mode(void) {
    return I2C_Read(FRAME_MODE) & (GET_SINGLE_FRAME | NO_HEADER | CONTINUOUS_STREAM);
}



//Multiplicative factor to the temperature readout of every pixel
void Set_Sensitivity_Factor(uint8_t sensitivity) {
    I2C_Write(SENSITIVITY_FACTOR, sensitivity);
}

uint8_t Get_Sensitivity_Factor(void) {
    return I2C_Read(SENSITIVITY_FACTOR);
}



//Emissivity value (percent) to be used in the conversion of raw data captured from SenXor to the temperature data
void Set_Emissivity(uint8_t emissivity) {
    I2C_Write(EMISSIVITY, emissivity);
}

uint8_t Get_Emissivity(void) {
    return I2C_Read(EMISSIVITY);
}   



//This register represents a temperature offset that will be added to every pixel in the data frame 
// before it is sent out to the host system. These values are in twos compliment.
void Set_Offset_Correction(uint8_t offset) {
    I2C_Write(OFFSET_CORR, offset);
}

uint8_t Get_Offset_Correction(void) {
    return I2C_Read(OFFSET_CORR);
}



void Set_Sleep_Mode(uint8_t sleep_mode) {
    uint8_t current_sleep_mode = I2C_Read(SLEEP_MODE);
    current_sleep_mode = (current_sleep_mode & 0b10000000) | (sleep_mode & 0b01111111);
    I2C_Write(SLEEP_MODE, current_sleep_mode);
}

void Go_To_Sleep(void) {
    uint8_t sleep_mode = I2C_Read(SLEEP_MODE);
    sleep_mode = sleep_mode | SLEEP;
    I2C_Write(SLEEP_MODE, sleep_mode);
}

void Wake_Up(void) {
    uint8_t sleep_mode = I2C_Read(SLEEP_MODE);
    sleep_mode = sleep_mode & ~SLEEP;
    I2C_Write(SLEEP_MODE, sleep_mode);
}

uint8_t Get_Sleep_Mode(void) {
    return I2C_Read(SLEEP_MODE);
}



void Half_Clock_Speed(void) {
    I2C_Write(CLK_SPEED, 0);
}

void Restore_Clock_Speed(void) {
    I2C_Write(CLK_SPEED, CLK_SLOW_DOWN);
}

uint8_t Get_Clock_Speed(void) {
    //A zero indicates half speed, a one indicates full speed.
    return I2C_Read(CLK_SPEED) & 0b00000001;
}



uint16_t Get_Firmware_Version_Build(void) {
    uint8_t fw_version_1 = I2C_Read(FW_VERSION_1);
    uint8_t fw_version_2 = I2C_Read(FW_VERSION_2);
    return (((fw_version_1 & 0x0F) << 12) | 
            ((fw_version_1 & 0xF0) << 4) |
              fw_version_2);
}

uint8_t Get_SenXor_Type(void) {
    return I2C_Read(SENSOR_TYPE) & 0b00011111;
}

uint64_t Get_SenXor_ID(void) {
    uint64_t accumulator = 0;
    for (int i = 0; i <= 5; i++) {
        accumulator = (accumulator << 8) | I2C_Read(SENXOR_ID_64 + i);
    }
    return accumulator;
}

