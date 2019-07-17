/*
usb_init.h
Initialize the USB controller
*/
#define F_CPU 16000000

#include "usb_init.h"
#include "avr/io.h"
#include <avr/interrupt.h>
#include <util/delay.h>

/*
The datasheet is wrong in that it says Endpoint 0, the control endpoint, stays configured through USB Bus Resets, such as device plug in. 
- -
Forum Post Identifying Problem & More Info: https://www.avrfreaks.net/forum/usb-initialization-problem
*/


ISR(USB_GEN_vect) {
	DDRC = 0xFF;
	PORTC = 0xFF;
}

int usb_init() {
	
	cli();
	UDIEN = 0;
	UEIENX = 0;
	sei();
	UHWCON |= (1 << UVREGE); // Enable USB Pads Regulator
	
	PLLCSR |= (1 << PINDIV); // Configure to use 16mHz oscillator
	PLLFRQ |= (1 << PLLUSB); // 96mHz USB Frequency setup
	PLLFRQ |= (1 << PLLTM1) | (1 << PLLTM0); // PLL Postscaler = 2

	PLLCSR |= (1 << PLLE); // Enable PLL

	while(!(PLLCSR & (1 << PLOCK))); // Wait for PLL Lock to be achieved

	USBCON |= (1 << USBE) | (1 << OTGPADE); // Enable USB Controller
	USBCON &= ~(1 << FRZCLK); // Unfreeze the clock

	UDCON = (1 << LSM); // High Speed Mode

	// Configure the Control Endpoint to receive setup packets
	
	UENUM |= 0; // Select Endpoint 0, the default control endpoint
	UECONX |= (1 << EPEN); // Enable the Endpoint
	UECFG0X = 0; // Control Endpoint, OUT direction for control endpoint
	UECFG1X |= (1 << EPSIZE1) | (1 << EPSIZE0) | (1 << ALLOC); // 64 byte endpoint, 1 bank, allocate the memory
	

	if(!(UESTA0X & (1 << CFGOK))) { // Check if endpoint configuration was successful
		return 1;
	}
		
	USBCON &= ~(1 << DETACH); // Connect
	UDIEN |= (1 << EORSTE);
	
	while(!(UEINTX & (1 << RXSTPI))); // Wait for the setup packet from the host
	
	return 0;
}
