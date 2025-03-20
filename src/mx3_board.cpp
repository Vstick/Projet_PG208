#include "mx3_board.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MX3 board //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MX3board::MX3board(const char* filename) {
    board_open(filename);
    sound = new Aux(this);
}

MX3board::~MX3board() {
    delete sound;
    board_close(tty);
}

Reg8 MX3board::operator[](unsigned char addr){
    return Reg8(this, addr);
}

void MX3board::writeEXT(unsigned char addr, char value) {
    (*this)[addr] = value;  // Accède directement à l'adresse et écrit dans le registre
}

MX3board& MX3board::operator<<(const std::string& message) {
    lcd->write(message);
    lcd->display(*this);
    return *this;
}

Aux& MX3board::getSound() {
    return *sound;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Reg8 ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reg8::Reg8(MX3board* card, unsigned char add) {
    this->board = card;
    this->addr = add;
}

void Reg8::operator=(unsigned char value) {
    board_d_write(board->tty, addr, value);
}

Reg8::operator unsigned char() const {
    return board_d_read(board->tty, addr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LCD ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LCD::LCD() {
    std::fill(std::begin(line1), std::end(line1), ' ');
    std::fill(std::begin(line2), std::end(line2), ' ');
}

void LCD::write(const std::string& message) {
    static int pos = 0;
    for (char c : message) {
        if (pos < 16) {
            line2[pos] = c;
        } else {
            for (int i = 0; i < 16; i++) {
                line1[i] = line2[i];
                line2[i] = ' ';
            }
            line2[0] = c;
            pos = 0;
        }
        pos++;
    }
}

void LCD::next() {
    for (int i = 0; i < 16; i++) {
        line1[i] = line2[i];
    }
    std::fill(std::begin(line2), std::end(line2), ' ');
}

void LCD::display(MX3board& board) {
    for (int i = 0; i < 16; i++) {
        board.writeEXT(MX3ADDR_LCD_START + i, line1[i]);
        board.writeEXT(MX3ADDR_LCD_START + 16 + i, line2[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Aux ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Aux::Aux(MX3board* brd) {
    board = brd;
}

Aux::~Aux() {}

void Aux::enable(bool state) {
    if (state)
        board->writeEXT(MX3ADDR_STATUS, MX3BIT_STATUS_AUD_EN | (*board)[MX3ADDR_STATUS]);
    else
        board->writeEXT(MX3ADDR_STATUS, ~MX3BIT_STATUS_AUD_EN & (*board)[MX3ADDR_STATUS]);
}

void Aux::setFreq(bool high) {
    if (high)
        board->writeEXT(MX3ADDR_STATUS, MX3BIT_STATUS_AUD_FREQ | (*board)[MX3ADDR_STATUS]);
    else
        board->writeEXT(MX3ADDR_STATUS, ~MX3BIT_STATUS_AUD_FREQ & (*board)[MX3ADDR_STATUS]);
}

uint16_t Aux::samplesInFIFO() {
    uint8_t low = (*board)[MX3ADDR_SND_ELTS_L];
    uint8_t high = (*board)[MX3ADDR_SND_ELTS_H];
    return (high << 8) | low;
}

void Aux::writeSample(uint8_t sample) {
    board->writeEXT(MX3ADDR_SND_WFIFO, sample);
}

Aux& Aux::operator<<(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Erreur : Impossible d'ouvrir le fichier audio");
    }

    enable(true); // Activation du son

    uint8_t sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        while (samplesInFIFO() >= 64000);
        writeSample(sample);
    }

    enable(false); // Désactivation du son à la fin
    return *this;
}
