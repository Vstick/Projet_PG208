#include "mx3_board.hpp"

MX3board::MX3board(const char* filename) {
    board_open(filename);
}

MX3board::~MX3board() {
    board_close(tty);
}

Reg8 MX3board::operator[](unsigned char addr){
    return Reg8(this, addr);
}



void Reg8::operator=(unsigned char value){
    board_d_write(board->tty, addr, value);
}

Reg8::operator unsigned char() const{
    return board_d_read(board->tty, addr);
}