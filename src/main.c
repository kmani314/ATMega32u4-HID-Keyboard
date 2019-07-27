/*
main.cpp
*/

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h> 
#include "usb.h"

int main(int argc, char** argv) {
	usb_init();
	while(!(get_usb_config_status));
	send_keypress(5, 0);

	while(1); // Wait a bit
}
