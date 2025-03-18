#ifndef COMMON_MX3_H
#define COMMON_MX3_H

#include <stdio.h>
#include <termios.h> // to access tty configuration
#include <unistd.h>  // write(), read(), close()
#include <fcntl.h>   // Contains file controls like O_RDWR

//register definitions
#include "mx3board_regs.h"

// extern int last_error;  // Déclaration de la variable globale

void board_close(int tty);
int board_open(const char* filename);
void board_d_write(int tty, unsigned char addr, unsigned char value);
unsigned char board_d_read(int tty, unsigned char addr);

#endif // COMMON_MX3_H
