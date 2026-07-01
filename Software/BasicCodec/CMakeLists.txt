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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(BasicCodec C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Hardware configuration definitions
set(OC_CLK_KHZ 153600)
set(SAMPLES_BIT_DEPTH 24)
set(SAMPLES_PER_SEC 50000.0)

# Add executable. Default name is the project name, version 0.1

add_executable(BasicCodec 
        src/BasicCodec.cpp
        src/CS4272.cpp)

pico_set_program_name(BasicCodec "BasicCodec")
pico_set_program_version(BasicCodec "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(BasicCodec 0)
pico_enable_stdio_usb(BasicCodec 1)

set(PIO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pio")
pico_generate_pio_header(BasicCodec ${PIO_DIR}/i2s.pio)

target_compile_definitions(BasicCodec PRIVATE
	SAMPLES_BIT_DEPTH=${SAMPLES_BIT_DEPTH}
        SAMPLES_PER_SEC=${SAMPLES_PER_SEC}f
        OC_CLK_KHZ=${OC_CLK_KHZ})

# Add the standard library to the build
target_link_libraries(BasicCodec
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(BasicCodec PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        include/)

# Add any user requested libraries
target_link_libraries(BasicCodec 
        hardware_i2c
        hardware_pio
        hardware_clocks)

pico_add_extra_outputs(BasicCodec)

