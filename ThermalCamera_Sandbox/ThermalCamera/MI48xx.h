#ifndef MI48XX_H
#define MI48XX_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"

// SPI Defines
#define SPI_PORT spi0
#define SPI_SPEED 10000*1000 //10 MHz
#define PIN_MISO 4
#define PIN_CS   5
#define PIN_SCK  2
#define PIN_MOSI 3
#define REP_TX_DATA 0x0000

// I2C defines
#define I2C_PORT i2c1
#define I2C_SPEED 100*1000 //100 KHz
#define I2C_SDA 6
#define I2C_SCL 7
#define I2C_DEVICE_ADDR 0x40

//GPIO Pin defines
#define READY_PIN 12
#define RESET_PIN 13

//Register addresses on the thermal camera

// Register and bit names       Address     Bit Masks     RW        Description
#define FRAME_MODE              0xB1                    //RW        Control the capture and readout of thermal data
    #define GET_SINGLE_FRAME                0b00000001
    #define CONTINUOUS_STREAM               0b00000010
    #define NO_HEADER                       0b00100000
#define FW_VERSION_1            0xB2                    //R         Firmware Version (Major, Minor)
#define FW_VERSION_2            0xB3                    //R         Firmware Version (Build)
#define FRAME_RATE              0xB4                    //RW        Frame rate
#define SLEEP_MODE              0xB5                    //RW        Control of power down parameters
    #define PERIOD_X100                     0b01000000
    #define SLEEP                           0b10000000
#define STATUS                  0xB6                    //R         Status of the attached camera module and MI48xx interface operations
    #define READOUT_TO_SLOW                 0b00000010
    #define SENXOR_IF_ERROR                 0b00000100
    #define CAPTURE_ERROR                   0b00001000
    #define DATA_READY                      0b00010000
    #define BOOTING_UP                      0b00100000
#define CLK_SPEED               0xB7                    //RW        Control of internal clock parameters
    #define CLK_SLOW_DOWN                   0b00000001
#define SENSOR_TYPE             0xBA                    //R         Type of the attached camera module
#define SENSITIVITY_FACTOR      0xC2                    //RW        Sensitivity correction factor
#define EMISSIVITY              0xCA                    //RW        Emissivity value for conversion of SenXorTM data to temperature
#define OFFSET_CORR             0xCB                    //RW        Fixed temperature shift of the entire frame for fine tuning the accuracy at product level
#define SENXOR_ID_64            0xE0                    //R         Production data and Serial number of the attached camera module (Spans 0xE0 through 0xE5)
                                                        //              0-7:      Production Year
                                                        //              8-15:     Production Week
                                                        //              16-23:    MANUF_LOCATION
                                                        //              24-63:    SERIAL_NUMBER
#define FILTER_CONTROL          0xD0                    //RW        Filter configuration and control
    #define TEMPORAL_ENABLE                 0b00000001
    #define TEMPORAL_INITIATE               0b00000010
    #define ROLL_AVG_ENABLE                 0b00000100
    #define MEDIAN_KERNEL_SELECT            0b00100000
    #define MEDIAN_ENABLE                   0b01000000
#define FILTER_SETTING_1        0xD1                    //RW        Parameters for the temporal filter (16 bit register)
                                                        //              0-7:      Temporal LSB        Least significant 8 bits of the filter strength
                                                        //              8-15:     Temporal_MSB        Most significant 8 bits of the filter strength
#define FILTER_SETTING_2        0xD3                    //RW        Parameters for the rolling average filter
#define USER_FLASH_CTRL         0xD8                    //RW        Enable/Disable host access to User Flash
    #define USER_FLASH_ENABLE               0b00000001


//Function Prototypes
void Start_SPI(void);
void Start_I2C(void);
void Setup_GPIO(void);
void Reset_Camera(void);
void SPI_Read(uint8_t include_header, uint16_t frame[62][80]);
void I2C_Write(uint8_t register_addr, uint8_t data);
uint8_t I2C_Read(uint8_t register_addr);

uint8_t Wait_For_Image_Ready(uint8_t timeout);

uint8_t Get_Status(void);

void Switch_Access_To_Flash(void);
void Switch_Access_To_Normal(void);

void Enable_Temporal_Filter(void);
void Disable_Temporal_Filter(void);
void Set_Temporal_Filter(uint16_t filter_strength);
uint16_t Get_Temporal_Filter(void);

void Enable_Rolling_Average_Filter(void);
void Disable_Rolling_Average_Filter(void);
void Set_Rolling_Average_Filter(uint8_t filter_strength);
uint8_t Get_Rolling_Average_Filter(void);

void Enable_Median_Filter(void);
void Disable_Median_Filter(void);
void Set_Median_Kernel(uint8_t kernel_size);
uint8_t Get_Median_Kernel(void);

void Set_Frame_Rate_Divider(uint8_t frame_rate);
uint8_t Get_Frame_Rate_Divider(void);

void Set_Frame_Mode(uint8_t frame_mode);
uint8_t Get_Frame_Mode(void);

void Set_Sensitivity_Factor(uint8_t sensitivity);
uint8_t Get_Sensitivity_Factor(void);

void Set_Emissivity(uint8_t emissivity);
uint8_t Get_Emissivity(void);

void Set_Offset_Correction(uint8_t offset);
uint8_t Get_Offset_Correction(void);

void Set_Sleep_Mode(uint8_t sleep_mode);
void Go_To_Sleep(void);
void Wake_Up(void);
uint8_t Get_Sleep_Mode(void);

void Half_Clock_Speed(void);
void Restore_Clock_Speed(void);
uint8_t Get_Clock_Speed(void);

uint16_t Get_Firmware_Version_Build(void);
uint8_t Get_SenXor_Type(void);
uint64_t Get_SenXor_ID(void);

#endif