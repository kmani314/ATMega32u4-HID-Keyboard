/*
usb.h
*/
#ifndef USB_H
#define USB_H
#include <stdbool.h>
#include <stdint.h>

static volatile uint8_t usb_config_status = 0;
static uint8_t keyboard_idle_value = 125; // HID Idle setting, how often the device resends unchanging reports, we are using a scaling of 4 because of the register size
static uint8_t current_idle = 0; // Counter that updates based on how many SOFE interrupts have occurred
static uint8_t this_interrupt = 0; // This is not the best way to do it, but it is much more readable than the alternative
static uint8_t keyboard_pressed_keys[6] = {0, 0, 0, 0, 0, 0};
static uint8_t keyboard_modifier = 0;
static uint8_t keyboard_protocol = 0; // This doesn't matter at all, we just need it for supporting some useless HID requests
static uint8_t keyboard_leds; // You don't need to use this if you don't want to or don't have the hardware

int usb_init();
bool get_usb_config_status();
int usb_send();
int send_keypress(uint8_t, uint8_t);

#define GET_STATUS 0x00
#define CLEAR_FEATURE 0x01
#define SET_FEATURE 0x03
#define SET_ADDRESS 0x05
#define GET_DESCRIPTOR 0x06
#define GET_CONFIGURATION 0x08
#define SET_CONFIGURATION 0x09
#define GET_INTERFACE 0x0A
#define SET_INTERFACE 0x0B

#define idVendor 0x16C0 // Atmel Corp.
#define idProduct 0x047C // ATMega32u4 DFU Bootloader (This isn't a real product so I don't have legitimate IDs)
#define KEYBOARD_ENDPOINT_NUM 3 // The second endpoint is the HID endpoint

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
