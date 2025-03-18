#include "common_mx3.hpp"

// int last_error = 0;


void board_close(int tty) {
    // closes the connection to the board
    // this method only takes care of things to be done
    // on the local computer, it does NOT handle any
    // closing operation on the board itself.
    printf("Closing board connection\n");
    close(tty);
}


int board_open(const char * filename) {
    // opens a TTY (UART) connection to the board
    // also sets communication parameters
    // returns a file description number (if positive) or an error code
    // if negative. Error codes :
    //     -1 : unable to open the tty descriptor
    //     -2 : unable to clean data received before the function call
    //     -3 : unable to clean data sent by previous program and not sent yet
    //     -4 : error reported while setting necessary parameters

    // opening the file entry
    int tty = open(filename, O_RDWR);
    if (tty == 0) {
        printf("Unable to connect to board\n");
        return -1;
    }
    printf("Board connection opened\n");

    // preparing the parameter set
    struct termios newtermios;
    newtermios.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;   // 8bits
    newtermios.c_iflag=IGNPAR;                          // no parity
    newtermios.c_oflag=0;                               // -- no other option --
    newtermios.c_lflag=0;                               // -- no other option --
    newtermios.c_cc[VMIN]=1;                            // 100 ms timeout
    newtermios.c_cc[VTIME]=0;                           // no delay for flush
    cfsetospeed(&newtermios,B115200);
    cfsetispeed(&newtermios,B115200);

    // clean input buffer
    if (tcflush(tty,TCIFLUSH)==-1) {
        printf("Unable to flush board connection input\n");
        board_close(tty);
        return -2;
    }
    // clean output buffer
    if (tcflush(tty,TCOFLUSH)==-1){
        printf("Unable to flush board connection output\n");
        board_close(tty);
        return -3;
    }
    // actually set parameters
    if (tcsetattr(tty,TCSANOW,&newtermios)==-1){
        printf("Unable to set connection parameters\n");
        board_close(tty);
        return -4;
    }

    // job's done !
    printf("Board connection ready\n");
    return tty;
}

void board_d_write(int tty, unsigned char addr, unsigned char data) {
    // write value 'data' at address 'addr'

    // prepare command
    unsigned char gen[3];
    gen[0] = MX3CMD_WR1;
    gen[1] = addr;
    gen[2] = data;

    // actually send command
    write(tty, gen, 3);
}

unsigned char board_d_read(int tty, unsigned char addr) {
    // read (and return) value stored at address 'addr'
    // if data is not received, simply returns 0, and set the global variable
    // 'last_error' to -1. to catch this error, user must clear 'last_error'
    // prior calling this function, then check 'last_error' value

    // this behavior is intentionnally not comfortable, you may provide
    // your own C++ specific behavior (or not)

    // prepare command
    unsigned char gen[2];
    gen[0] = MX3CMD_RD1;
    gen[1] = addr;

    // actually send command
    write(tty, gen, 2);

    // retreive data from board (1 byte)
    if (!read(tty, gen, 1)) {
        // read returned 0 (no data received)
        printf("No answer from board\n");
        // last_error = -1;
        return 0;
    }
    return gen[0];
}



// int main(int argc, char *argv[]) {
//     // check that we received the filename of the tty connection
//     if (argc<2) {
//         printf("No device name provided\n");
//         return -1;
//     }

//     // opens the connection
//     int tty = board_open(argv[1]);
//     if (tty<0) {
//         return -1;
//     }

//     // set even numbered LEDs, clear odd numbered LEDs
//     board_d_write(tty, MX3ADDR_LED, 0x55);

//     // retreive switches values (and display it)
//     last_error = 0;
//     unsigned char sw = board_d_read(tty, MX3ADDR_SW);
//     if (last_error == 0) {
//         printf("Switches value : 0x%X\n", sw);
//     }

//     // close board
//     board_close(tty);
//     return 0;
// }