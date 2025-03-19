#include "mx3_board.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MX3 board //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MX3board::MX3board(const char* filename) {
    board_open(filename);
}

MX3board::~MX3board() {
    board_close(tty);
}

Reg8 MX3board::operator[](unsigned char addr){
    return Reg8(this, addr);
}

void MX3board::writeLCD(unsigned char addr, char value) {
    (*this)[addr] = value;  // Accède directement à l'adresse et écrit dans le registre
}

MX3board& MX3board::operator<<(const std::string& message) {
    lcd->write(message);  // Appel de la méthode write sur LCD pour écrire le message
    lcd->display(*this);       // Affichage du texte à l'écran
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Reg8 ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reg8::Reg8(MX3board* card, unsigned char add){
    this->board = card;
    this->addr = add;
}

void Reg8::operator=(unsigned char value){
    board_d_write(board->tty, addr, value);
}

Reg8::operator unsigned char() const{
    return board_d_read(board->tty, addr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LCD ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LCD::LCD(){
    // Initialisation des lignes LCD à vide
    std::fill(std::begin(line1), std::end(line1), ' ');
    std::fill(std::begin(line2), std::end(line2), ' ');
}

void LCD::write(const std::string& message){
    static int pos = 0;
    for (char c : message) {
        if (pos < 16) {
            line2[pos] = c;
        } else {
            // Quand la ligne 2 est pleine, on déplace la ligne 2 dans la ligne 1
            for (int i = 0; i < 16; i++) {
                line1[i] = line2[i];
                line2[i] = ' ';
            }
            line2[0] = c; // Remise du caractère dans la ligne 2
            pos = 0;
        }
        pos++;
    }
}

void LCD::next(){
    for (int i = 0; i < 16; i++) {
        line1[i] = line2[i];
    }
    std::fill(std::begin(line2), std::end(line2), ' ');
}

void LCD::display(MX3board& board){
    for (int i = 0; i < 16; i++) {
        board.writeLCD(MX3ADDR_LCD_START + i, line1[i]);
        board.writeLCD(MX3ADDR_LCD_START + 16 + i, line2[i]);
    }
}