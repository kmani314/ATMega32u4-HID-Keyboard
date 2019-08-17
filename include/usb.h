/*
usb.h
*/
#ifndef USB_H
#define USB_H
#include <stdbool.h>
#include <stdint.h>


extern volatile uint8_t keyboard_pressed_keys[6];
extern volatile uint8_t keyboard_modifier;

uint8_t usb_config_status;
uint8_t keyboard_protocol; // This doesn't matter at all, we just need it for supporting a request

uint8_t keyboard_leds;  // You don't need to use this if you don't want
                               // to or don't have the hardware

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

#define idVendor 0x03eb  // Atmel Corp.
#define idProduct 0x2ff4  // ATMega32u4 DFU Bootloader (This isn't a real product so I don't
          // have legitimate IDs)
#define KEYBOARD_ENDPOINT_NUM 3  // The second endpoint is the HID endpoint

#define CONFIG_SIZE 34
#define HID_OFFSET 18

// HID Class-specific request codes - refer to HID Class Specification
// Chapter 7.2 - Remarks

#define GET_REPORT 0x01
#define GET_IDLE 0x02
#define GET_PROTOCOL 0x03
#define SET_REPORT 0x09
#define SET_IDLE 0x0A
#define SET_PROTOCOL 0x0B

#endif
