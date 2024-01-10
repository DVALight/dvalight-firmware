cmake_minimum_required(VERSION 3.16)

# STM32_TOOLCHAIN_PATH, STM32_TARGET_TRIPLET, STM32_CUBE_F4_PATH must be present in environment variables
# check https://github.com/DVALight/stm32-cmake for more info

project(dvalight-firmware C ASM)
set(SOURCES
    src/core/syscalls.c
    src/core/sysmem.c
    src/hal_isr.c
    src/hal_msp.c
    src/uart.c
    src/network.c
    src/main.c
)

set(HEADERS
    src/stm32f4xx_hal_conf.h
    src/hal_isr.h
    src/uart.h
    src/network.h
)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

#set(HAL_COMP_LIST RCC GPIO CORTEX)
list(APPEND HAL_COMP_LIST STM32F4 RCC GPIO UART SPI CORTEX)
list(APPEND CMSIS_COMP_LIST "STM32F4")

if(NOT STM32_CMAKE_PACKAGES)
    find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
    find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)
    set(STM32_CMAKE_PACKAGES 1)
endif()

add_subdirectory(../stm32-enc28j60 stm32-enc28j60)

add_executable(dvalight-firmware ${SOURCES} ${HEADERS})
# we do this to hide sensitive information like MAC address
target_compile_definitions(dvalight-firmware PRIVATE
    LOCAL_MAC_STR="02:03:04:d4:c8:10"
    LOCAL_IP_STR="192.168.100.169"
    REMOTE_MAC_STR="$ENV{LOCAL_MAC}"
    REMOTE_IP_STR="$ENV{LOCAL_IP}" 
)

target_include_directories(dvalight-firmware PRIVATE
    src
)

target_link_libraries(dvalight-firmware
    stm32-enc28j60
    HAL::STM32::F4::RCC
    HAL::STM32::F4::GPIO
    HAL::STM32::F4::UART
    HAL::STM32::F4::SPI
    HAL::STM32::F4::CORTEX
    CMSIS::STM32::F401VE
    STM32::NoSys
)

add_custom_command(TARGET dvalight-firmware POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex dvalight-firmware.elf dvalight-firmware.hex
)

stm32_print_size_of_target(dvalight-firmware)