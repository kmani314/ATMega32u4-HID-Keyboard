/*
main.cpp
*/

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h> 
#include "usb.h"
#include <string.h>
#include "matrix.h"

int main(int argc, char** argv) {
	usb_init();
	
	DDRC |= ((1 << 7) | (1 << 6));
	
	while(!get_usb_config_status()) {
		// LED Animation
		PORTC &= ~(1 << 6);
		PORTC |= (1 << 7);
		_delay_ms(100);
		PORTC &= ~(1 << 7);
		PORTC |= (1 << 6);
		_delay_ms(100);

	}
	PORTC &= ~((1 << 6) | (1 << 7));

	_delay_ms(500);
	
/*	DDRD |= 0xFF;
	PORTD &= 0;
	DDRF |= 0xFF;
	PORTF &= 0;

	DDRB &= 0;
	PORTB = 0xFF;
	
	while(1) {
		if(PINB != 0xFF) {
			PORTC |= (1 << 6);
		} else {
			PORTC = 0;
		}
	}*/
	
	matrix_init();
	while(1) {
		do_matrix_scan();
	}
}
