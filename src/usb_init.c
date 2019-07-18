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

int usb_init() {
	
	cli(); // Global Interrupt Disable
	
	UHWCON |= (1 << UVREGE); // Enable USB Pads Regulator
	
	PLLCSR |= (1 << PINDIV); // Configure to use 16mHz oscillator

	PLLCSR |= (1 << PLLE); // Enable PLL

	while(!(PLLCSR & (1 << PLOCK))); // Wait for PLL Lock to be achieved

	USBCON |= (1 << USBE) | (1 << OTGPADE); // Enable USB Controller
	USBCON &= ~(1 << FRZCLK); // Unfreeze the clock

	UDCON = (1 << LSM); // High Speed Mode (Full 12mbps)

	USBCON &= ~(1 << DETACH); // Connect
	UDIEN |= (1 << EORSTE) | (1 << SOFE); // Re-enable the EORSTE (End Of Reset) Interrupt so we know when we can configure the control endpoint
	
	sei();
	while(1); // Wait for the setup packet from the host

	return 0;
}

ISR(USB_GEN_vect) {
	if(UDINT & (1 << EORSTI)) {// If end of reset interrupt	
		UDINT = 0;
		// Configure Control Endpoint
		UENUM = 0; // Select Endpoint 0, the default control endpoint
		UECONX = (1 << EPEN); // Enable the Endpoint
		UECFG0X = 0; // Control Endpoint, OUT direction for control endpoint
		UECFG1X |= 0x32; // 64 byte endpoint, 1 bank, allocate the memory
		
		if(!(UESTA0X & (1 << CFGOK))) { // Check if endpoint configuration was successful
			return;	
		}
			
		UERST = 1;
		UERST = 0;
		
		UEIENX = (1 << RXSTPE); // Re-enable the RXSPTE (Receive Setup Packet) Interrupt
	
	}
}

ISR(USB_COM_vect) {
	UENUM = 0;
	if(UEINTX & (1 << RXSTPI)) {
		UEINTX &= ~(1 << RXSTPI);
		// Setup Packet Received
		DDRC = 0xFF;
		PORTC = 0xFF;
	}
}

