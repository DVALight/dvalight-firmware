cmake_minimum_required(VERSION 3.16)

# STM32_TOOLCHAIN_PATH, STM32_TARGET_TRIPLET, STM32_CUBE_F4_PATH must be present in environment variables
# check https://github.com/DVALight/stm32-cmake for more info

project(dvalight-firmware C ASM)
set(SOURCES
    src/core/syscalls.c
    src/core/sysmem.c
    src/hal_isr.c
    src/hal_msp.c
    src/main.c
    src/uart.c
)

set(HEADERS
    src/stm32f4xx_hal_conf.h
    src/hal_isr.h
    src/uart.h
)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

#set(HAL_COMP_LIST RCC GPIO CORTEX)
list(APPEND HAL_COMP_LIST STM32F4 RCC GPIO UART CORTEX)
list(APPEND CMSIS_COMP_LIST "STM32F4")

if(NOT STM32_CMAKE_PACKAGES)
    find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
    find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)
    set(STM32_CMAKE_PACKAGES 1)
endif()

add_executable(dvalight-firmware ${SOURCES} ${HEADERS})
target_include_directories(dvalight-firmware PRIVATE # change to PUBLIC if you're making library
    src
)

target_link_libraries(dvalight-firmware
    HAL::STM32::F4::RCC
    HAL::STM32::F4::GPIO
    HAL::STM32::F4::UART
    HAL::STM32::F4::CORTEX
    CMSIS::STM32::F401VE
    STM32::NoSys
)

add_custom_command(TARGET dvalight-firmware POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex dvalight-firmware.elf dvalight-firmware.hex
)

stm32_print_size_of_target(dvalight-firmware)