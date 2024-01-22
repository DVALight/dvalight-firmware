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
    src/dva.c
    src/main.c
)

set(HEADERS
    src/stm32f4xx_hal_conf.h
    src/hal_isr.h
    src/uart.h
    src/network.h
    src/dva.h
)

# set(DVA_LOCAL_MAC   "$ENV{LOCAL_MAC}"   CACHE INTERNAL "MAC for DVALight device")
set(DVA_LOCAL_MAC   "02:03:04:69:69:69"   CACHE INTERNAL "MAC for DVALight device")
set(DVA_LOCAL_IP    "192.168.100.169"     CACHE INTERNAL "IPv4 address for DVALight device")
set(DVA_GATEWAY_IP  "192.168.100.1"       CACHE INTERNAL "gateway IPv4 address")
set(DVA_GATEWAY_MAC "$ENV{LOCAL_MAC}"     CACHE INTERNAL "MAC for gateway")
set(DVA_REMOTE_IP   "$ENV{LOCAL_IP}"      CACHE INTERNAL "back-end IPv4 address")
set(DVA_SPORT       "61337"               CACHE INTERNAL "DVALight device UDP source port")
set(DVA_DPORT       "1337"                CACHE INTERNAL "back-end UDP port")
set(DVA_NETBUF_SIZE "1536"                CACHE INTERNAL "buffer memory size for network packets")

set(DVA_DELAY       "500"                 CACHE INTERNAL "delay between DVA requests")
set(DVA_DEVICE_ID   "1"                   CACHE INTERNAL "device ID for DVA protocol")

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

#set(HAL_COMP_LIST RCC GPIO CORTEX)
list(APPEND HAL_COMP_LIST STM32F4 RCC GPIO UART SPI TIM CORTEX)
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
  DVA_LOCAL_MAC="${DVA_LOCAL_MAC}"
  DVA_LOCAL_IP="${DVA_LOCAL_IP}"
  DVA_GATEWAY_IP="${DVA_GATEWAY_IP}"
  DVA_GATEWAY_MAC="${DVA_GATEWAY_MAC}"
  DVA_REMOTE_MAC="${DVA_REMOTE_MAC}"
  DVA_REMOTE_IP="${DVA_REMOTE_IP}"
  DVA_SPORT=${DVA_SPORT}
  DVA_DPORT=${DVA_DPORT}
  DVA_NETBUF_SIZE=${DVA_NETBUF_SIZE}
  
  DVA_DELAY=${DVA_DELAY}
  DVA_DEVICE_ID=${DVA_DEVICE_ID}
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
    HAL::STM32::F4::TIM
    HAL::STM32::F4::TIMEx
    HAL::STM32::F4::CORTEX
    CMSIS::STM32::F401VE
    STM32::NoSys
)

add_custom_command(TARGET dvalight-firmware POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex dvalight-firmware.elf dvalight-firmware.hex
)

stm32_print_size_of_target(dvalight-firmware)