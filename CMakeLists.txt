cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(blink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(blink main.cpp sensors.cpp virtualserial.cpp)

target_link_libraries(blink pico_stdlib hardware_i2c)

pico_add_extra_outputs(blink)
