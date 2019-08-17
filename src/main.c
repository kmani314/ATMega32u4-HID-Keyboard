/*
main.c
*/

#define F_CPU 16000000

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include "matrix.h"
#include "usb.h"

int main(int argc, char** argv) {
  usb_init();

  DDRC |= ((1 << 7) | (1 << 6));

  while (!get_usb_config_status()) {
    // LED Animation
    PORTC &= ~(1 << 6);
    PORTC |= (1 << 7);
    _delay_ms(100);
    PORTC &= ~(1 << 7);
    PORTC |= (1 << 6);
    _delay_ms(100);
  }
  PORTC &= ~((1 << 6) | (1 << 7));

  matrix_init();
	while(1) do_matrix_scan(); // Scan the matrix
}
