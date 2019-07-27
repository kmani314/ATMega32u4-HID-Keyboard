/*
usb.h
*/
#ifndef USB_H
#define USB_H
#include <stdbool.h>
#include <stdint.h>

static volatile bool usb_config_status = 0;
static uint8_t keyboard_idle_value = 125; // HID Idle setting, how often the device resends unchanging reports, we are using a scaling of 4 because of the register size
static uint8_t current_idle = 0; // Counter that updates based on how many SOFE interrupts have occurred
static uint8_t keyboard_pressed_keys[6] = {};
static uint8_t keyboard_modifier;
static uint8_t keyboard_protocol = 0; // This doesn't matter at all, we just need it for supporting some useless HID requests
static uint8_t keyboard_leds; // You don't need to use this if you don't want to or don't have the hardware
int usb_init();

#define GET_STATUS 0x00
#define CLEAR_FEATURE 0x01
#define SET_FEATURE 0x03
#define SET_ADDRESS 0x05
#define GET_DESCRIPTOR 0x06
#define SET_DESCRIPTOR 0x07
#define GET_CONFIGURATION 0x08
#define SET_CONFIGURATION 0x09
#define GET_INTERFACE 0x0A
#define SET_INTERFACE 0x0B

#define idVendor 0x03eb // Atmel Corp.
#define idProduct 0x2ff4 // ATMega32u4 DFU Bootloader (This isn't a real product so I don't have legitimate IDs)

#define KEYBOARD_ENDPOINT_NUM 2 // The second endpoint is the HID endpoint

#define CONFIG_SIZE 34
#define HID_OFFSET 18

// HID Class-specific request codes - refer to HID Class Specification Chapter 7.2 - Remarks

#define GET_REPORT 0x01
#define GET_IDLE 0x02
#define GET_PROTOCOL 0x03
#define SET_REPORT 0x09
#define SET_IDLE 0x0A
#define SET_PROTOCOL 0x0B

#endif
