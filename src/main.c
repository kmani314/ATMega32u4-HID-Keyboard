/*
main.cpp
*/

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h> 
#include "usb.h"
#include "keys.h"
#include <string.h>

static const char pasta[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890!@#$%^&*()`~-_=+[]{}\\|;:\'\",<.>/?";

int convertChar(char c) {
	int ascii = (int)c;
	return keycodes[ascii - ASCII_OFFSET];
}

int getModifier(char c) {
	int ascii = (int)c;
	return modifiers[ascii - ASCII_OFFSET];
}

int main(int argc, char** argv) {
	usb_init();
	while(!get_usb_config_status());
	_delay_ms(1000);
	int a;
	for(int i = 0; i < strlen(pasta); i++) {
		a = send_keypress(convertChar(pasta[i]), getModifier(pasta[i]));
		_delay_ms(50);
	}
}
