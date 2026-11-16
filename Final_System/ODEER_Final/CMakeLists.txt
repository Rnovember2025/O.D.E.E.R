# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ODEER_Final C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(ODEER_Final ODEER_Final.c I2C_To_ESP32.c HotspotDetection.c Thermal_Camera_Main.c MI48xx.c)

pico_set_program_name(ODEER_Final "ODEER_Final")
pico_set_program_version(ODEER_Final "0.1")

# Use the standard flash-backed Pico binary type for a normal program image.
# In this SDK version, "flash" is not a valid binary type name; use "default".
pico_set_binary_type(ODEER_Final default)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ODEER_Final 0)
pico_enable_stdio_usb(ODEER_Final 1)

# Add the standard library to the build
target_link_libraries(ODEER_Final
        pico_stdlib
        hardware_i2c
        hardware_spi
        pico_multicore)

# Add the standard include files to the build
target_include_directories(ODEER_Final PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(ODEER_Final)

