# CMake for AVR ATMega32u4
# Written by Krishna Mani, July 2019

cmake_minimum_required(VERSION 3.1)

set(build_target atmega32u4)

set(CMAKE_C_COMPILER /usr/local/bin/avr-gcc)
set(CMAKE_C_FLAGS -mmcu=${build_target})

project(ATMega32u4-HID-Keyboard)

file(GLOB_RECURSE SOURCES src/*.c)

set(CMAKE_C_LINK_FLAGS)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)

add_executable(keyboard ${SOURCES})

include_directories(keyboard /usr/local/Cellar/avr-gcc/9.1.0/avr/include/)
include_directories(keyboard include/)
