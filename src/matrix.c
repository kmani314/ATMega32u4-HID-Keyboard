#include "matrix.h"
#include "usb.h"
#include "layout.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>

int layout_num = 0;

bool state_layer[NUM_ROWS][NUM_COLS] = 
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
}; // Key States to do state changes

int matrix_init() {
	DDRD = 0xFF; // Configure the first 7 rows as outputs
	DDRF = 0xFF; // Configure the last 5 as outputs
	PORTF = 0xFF; // The logic is inverted, being pulled low is a keypress
	PORTD = 0xFF;
	
	DDRB &= 0; // Configure the row port as inputs
	PORTB = 0xFF; // Enable the Pull up resistors
}
void do_layer_led() {
	PORTC = layout_num << 6;
}
int do_matrix_scan() {
	bool key_state;

	for(int i = 0; i < NUM_ROWS; i++) {
		_delay_ms(10);
		for(int j = 0; j < NUM_COLS; j++) {
			_delay_ms(10);
			if(j < 7) {
				// This is because of the dumb routing on PORTD
				int DNUM;
				switch(j) {
					case(0):
						DNUM = 0;
						break;
					case(1):
						DNUM = 6;
						break;
					case(2):
						DNUM = 4;
						break;
					case(3):
						DNUM = 5;
						break;
					case(4):
						DNUM = 3;
						break;
					case(5):
						DNUM = 2;
						break;
					case(6):
						DNUM = 1;
						break;
				}
				
				PORTD &= ~(1 << DNUM); // Set it low to read from the row
			} else { // PORTF is not continuous, which means we cannot just loop over it
				int FNUM = j - 7;
				if(FNUM > 1) FNUM += 2; // Bits 2 and 3 do not exist on PORTF
				PORTF &= ~(1 << FNUM);
			}
				
			if((bool)(PINB & (1 << (i + B_OFFSET))) != state_layer[i][j]) { // state change 
				if(!(PINB & (1 << (i + B_OFFSET)))) { // Low to High (Key Down)
					uint16_t key = layout[layout_num][i][j];
					if(key & KEY_MOD_MACRO) {
						key &= ~(KEY_MOD_MACRO);
						keyboard_modifier |= key;
						
					} else if(key & KEY_LS_MACRO) {
						key &= ~(KEY_LS_MACRO);
						layout_num |= key;
						do_layer_led();
					} else {
						for(int i = 0; i < 6; i ++) {
							if(!keyboard_pressed_keys[i]) {
								keyboard_pressed_keys[i] = key;
								break;
							}
						}
					}
				} else {
					uint16_t key = layout[layout_num][i][j];
					if(key & KEY_MOD_MACRO) {
						key &= ~(KEY_MOD_MACRO);
						keyboard_modifier &= ~(key);
						
					} else if(key & KEY_LS_MACRO) {
						key &= ~(KEY_LS_MACRO);
						layout_num &= ~(key);
						do_layer_led();
					} else {
						for(int i = 0; i < 6; i ++) {
							if(keyboard_pressed_keys[i] == key) {
								keyboard_pressed_keys[i] = 0;
								break;
							}
						}
					}
				}
				usb_send();
				state_layer[i][j] = (PINB & (1 << (i + B_OFFSET))); // save the state
			
			}
				
			PORTF = 0xFF;
			PORTD = 0xFF;
		}
	}
}
/*
ISR(TIMER0_OVF_vect) {
	do_matrix_scan();	
}*/
