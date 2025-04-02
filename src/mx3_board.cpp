#include "mx3_board.hpp"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MX3 board //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MX3board::MX3board(const char* filename) {
    tty = board_open(filename);
    sound = new Aux(this);
    lcd = new LCD();
    sound->setFreq(false);
}

MX3board::~MX3board() {
    delete sound;
    delete lcd;
    board_close(tty);
}

Reg8 MX3board::operator[](unsigned char addr){
    return Reg8(this, addr);
}

void MX3board::writeEXT(unsigned char addr, char value) {
    (*this)[addr] = value;
}

MX3board& MX3board::operator<<(const std::string& message) {
    lcd->writeLCD(message);
    lcd->display(*this);
    lcd->next();
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
    unsigned char gen[3];
    gen[0] = MX3CMD_WR1;
    gen[1] = addr;
    gen[2] = value;

    // actually send command
    write(board->tty, gen, 3);
}

Reg8::operator unsigned char() const {
    unsigned char gen[2];
    gen[0] = MX3CMD_RD1;
    gen[1] = addr;

    // actually send command
    write(board->tty, gen, 2);

    // retreive data from board (1 byte)
    if (!read(board->tty, gen, 1)) {
        // read returned 0 (no data received)
        printf("No answer from board\n");
        return 0;
    }
    return gen[0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LCD ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LCD::LCD() {
    std::fill(std::begin(line1), std::end(line1), ' ');
    std::fill(std::begin(line2), std::end(line2), ' ');
}

void LCD::writeLCD(const std::string& message) {
    int pos = 0;
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
    unsigned char gen[35];
    gen[0] = MX3CMD_WR_MAP;
    gen[1] = MX3ADDR_LCD_START;
    gen[2] = 32;
    for (int i = 0; i < 16; i++) {
        gen[3 + i] = line1[i];
        gen[19 + i] = line2[i];
    }
    write(board.tty, gen, 35);
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
        (*board)[MX3ADDR_STATUS] = MX3BIT_STATUS_AUD_EN | (*board)[MX3ADDR_STATUS];
    else
        (*board)[MX3ADDR_STATUS] = ~MX3BIT_STATUS_AUD_EN & (*board)[MX3ADDR_STATUS];
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

uint8_t Aux::writeSample(std::ifstream& file) {  
    uint8_t sample;
    file.read(reinterpret_cast<char*>(&sample), sizeof(sample));
    return sample;
}

void Aux::writeFIFO(uint16_t size, std::ifstream& file){
    uint16_t cnt{0};
    
    unsigned char gen[255];
    gen[0] = MX3CMD_WR_FF;
    gen[1] = MX3ADDR_SND_WFIFO;
    gen[2] = 252;

    while(size >= cnt){
        for(int i{3}; i<255; i++){
            if(file.eof()){
                return;
            }
            gen[i] = writeSample(file);
        }

        // actually send command
        write(board->tty, gen, 255);
        cnt += 255;
    }
}

void Aux::play(const std::string& filename) {
    if (musicThread.joinable()) {
        stopMusic = true;
        musicThread.join();  // Attendre la fin de l'ancien thread
    }

    stopMusic = false;  // Réinitialiser le flag d'arrêt

    musicThread = std::thread([this, filename]() {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "Erreur : Impossible d'ouvrir le fichier audio" << std::endl;
            return;
        }

        writeFIFO(64000, file);
        enable(true);

        while (!file.eof() && !stopMusic) {
            if (samplesInFIFO() <= 32000) {
                writeFIFO(32000, file);
            }
        }

        // Attente pour vider le FIFO
        while (samplesInFIFO() > 0 && !stopMusic);

        enable(false); // Désactivation du son à la fin
    });
}

void Aux::stop() {
    stopMusic = true;
    if (musicThread.joinable()) {
        musicThread.join();  // Attendre que le thread se termine
    }
}

Aux& Aux::operator<<(const std::string& filename) {
    play(filename); // Appel à la méthode play() pour démarrer la musique
    return *this;
}

Aux& Aux::operator<<(const char* filename) {
    return *this << std::string(filename);
}
