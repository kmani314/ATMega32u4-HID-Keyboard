/*
main.cpp
*/

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h> 
#include "usb.h"

int main(int argc, char** argv) {
	usb_init();
	while(1); // temporary to make sure it doesn't exit
}
