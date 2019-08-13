#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>

#define NUM_ROWS 4 
#define NUM_COLS 12
#define B_OFFSET 4 // PORTB does not start at 0, 0-3 is used for ICSP


int matrix_init();
int do_matrix_scan();
#endif
