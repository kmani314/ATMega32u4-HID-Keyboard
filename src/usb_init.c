/*
usb_init.h
Initialize the USB controller
*/
#include "usb_init.h"
#include "avr/io.h"
#include <avr/interrupt.h>

int usb_init() {
	cli();
	UHWCON |= (1 << UVREGE); // Enable USB Pads Regulator
	
	return 0;
}
