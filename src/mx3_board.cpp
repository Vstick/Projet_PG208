#include "mx3_board.hpp"

MX3board::MX3board(const char* filename) {
    board_open(filename);
    led = new LEDs(this);
}

MX3board::~MX3board() {
    board_close(tty);
    delete led;
}

Reg8 MX3board::operator[](unsigned char addr){
    return Reg8(this, addr);
}


/////////////////////////////////////////////////////// Reg8 ////////////////////////////////////////////////////////////
void Reg8::operator=(unsigned char value){
    board_d_write(board->tty, addr, value);
}

Reg8::operator unsigned char() const{
    return board_d_read(board->tty, addr);
}

Reg16 Reg8::r16() {
    return Reg16(*this, Reg8(board, addr + 1));  //Reg8 pointe sur l’adresse mémoire suivante
}

void init(MX3board* card, unsigned char add){
    Reg8->board = card;
    Reg8->addr = add;
    return 0;
}

Bit Reg8::operator[](int a) {
   return Bit(*this, a);
}


////////////////////////////////////////////////////////// Bit ////////////////////////////////////////////////////////////
void Bit::operator=(int pos){
    unsigned char t = reg;
    if (valeur)
        t |= (1 << bit_pos);        //Met le bit concerné à 1
    else
        t &= ~(1 << bit_pos);       //Met le bit concerné à 0
    reg = t;
}

Bit Bit::operator bool(){
    return (reg & (1 << bit_pos)) != 0;     //Renvoie la valeur du bit
}


////////////////////////////////////////////////////// Reg16 ////////////////////////////////////////////////////////////////
void Reg16::operator=(unsigned short v16) {
    lsb = (unsigned char)v16;     //Convertit en unsigned char
    msb = (unsigned char)(v16 >> 8);     //On prend que les 8 bits de poids fort
}

void Reg16::operator unsigned short() {
    return (unsigned short)(msb << 8) | lsb;       //Ou avec les bits de poids faible et de poids fort
}

////////////////////////////////////////////////////// 7seg //////////////////////////////////////////////////////////////////
int 7seg::char2int(char data) {
    switch (data) {
        case '0': return 0x3F;
        case '1': return 0x06;
        case '2': return 0x5B;
        case '3': return 0x4F;
        case '4': return 0x66;
        case '5': return 0x6D;
        case '6': return 0x7D;
        case '7': return 0x07;
        case '8': return 0x7F;
        case '9': return 0x6F;
        case 'A': case 'a': return 0x77;
        case 'B': case 'b': return 0x7C;
        case 'C': case 'c': return 0x39;
        case 'D': case 'd': return 0x5E;
        case 'E': case 'e': return 0x79;
        case 'F': case 'f': return 0x71;
        default: return 0;  // Si le caractère n'est pas supporté
    }
}

// Affichage d'un nombre entier
void 7seg::affich(int value, bool hex) {
    if (hex) {
        brd[MX3ADDR_7SEG_HEX_L] = (unsigned char)(v alue & 0xFF);
        brd[MX3ADDR_7SEG_HEX_H] = (unsigned char)((value >> 8) & 0xFF);
    } else {
        brd[MX3ADDR_7SEG_DEC_L] = (unsigned char)(value & 0xFF);
        brd[MX3ADDR_7SEG_DEC_H] = (unsigned char)((value >> 8) & 0xFF);
    }
}
 


/////////////////////////////////////////////////////////// LEDs ////////////////////////////////////////////////////////////////
void LEDs::operator=(MX3board* sw) {
    brd[MX3ADDR_LED] = sw[MX3ADDR_SW];  //écrit la valeur du switch sur le registre LED
}