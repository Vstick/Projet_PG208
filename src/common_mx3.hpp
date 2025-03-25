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

#endif // COMMON_MX3_H
