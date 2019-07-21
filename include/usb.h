/*
usb_init.h
*/
#ifndef USB_INIT_H
#define USB_INIT_H

static volatile bool usb_config_status;
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

#define idVendor 0x16C0 // Teensy
#define idProduct 0x047C // Teensy Keyboard (This isn't a real product so I don't have legitimate IDs):q

#endif
