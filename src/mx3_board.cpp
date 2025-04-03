#include "mx3_board.hpp"
#include <iostream>

// The commentary was added by ChatGPT

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MX3 board //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor for the MX3board class
MX3board::MX3board(const char* filename) {
    tty = board_open(filename); // Open the board connection
    sound = new Aux(this); // Initialize the Aux (sound) component
    lcd = new LCD();       // Initialize the LCD component
    sound->setFreq(false); // Set the default frequency for sound
    led = new LEDs(this);
}

// Destructor for the MX3board class
MX3board::~MX3board() {
    delete sound;          // Clean up the sound component
    delete lcd;            // Clean up the LCD component
    board_close(tty);      // Close the board connection
    delete led;
}

// Overload operator[] to access registers
Reg8 MX3board::operator[](unsigned char addr){
    return Reg8(this, addr);
}

// Overload operator<< to send a message to the LCD
MX3board& MX3board::operator<<(const std::string& message) {
    lcd->writeLCD(message);                 // Write the message to the LCD
    lcd->display(*this);                    // Display the message on the LCD
    lcd->next();                            // Move to the next line
    return *this;
}

// Get the Aux (sound) component
Aux& MX3board::getSound() {
    return *sound;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Reg8 ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor for the Reg8 class
Reg8::Reg8(MX3board* card, unsigned char add) {
    this->board = card; // Associate the register with the board
    this->addr = add;   // Set the register address
}

// Overload operator= to write a value to the register
void Reg8::operator=(unsigned char value) {
    unsigned char gen[3];
    gen[0] = MX3CMD_WR1; // Command to write 1 byte
    gen[1] = addr;       // Register address
    gen[2] = value;      // Value to write

    // Send the command to the board
    write(board->tty, gen, 3);
}

// Overload operator unsigned char to read a value from the register
Reg8::operator unsigned char() const {
    unsigned char gen[2];
    gen[0] = MX3CMD_RD1; // Command to read 1 byte
    gen[1] = addr;       // Register address

    // Send the command to the board
    write(board->tty, gen, 2);

    // Retrieve data from the board (1 byte)
    if (!read(board->tty, gen, 1)) {
        // If no data is received
        printf("No answer from board\n");
        return 0;
    }
    return gen[2];
}

Reg16 Reg8::r16() { 
    return Reg16(this, new Reg8(board, addr + 1));  //Reg8 pointe sur l’adresse mémoire suivante
}

Bit Reg8::operator[](int a) {
   return Bit(this, a);
}

////////////////////////////////////////////////////////// Bit ////////////////////////////////////////////////////////////
void Bit::operator=(int pos){
    unsigned char reg_bit = *reg;  // Lire la valeur actuelle du registre
    if (reg_bit & (1 << pos))
        reg_bit |= (1 << bit_pos);        //Met le bit concerné à 1
    else
        reg_bit &= ~(1 << bit_pos);       //Met le bit concerné à 0
    *reg = reg_bit; // Write the modified value back to the register
}

Bit::operator bool(){
    return (*reg & (1 << bit_pos)) != 0;     //Renvoie la valeur du bit
}

Bit::~Bit(){}

////////////////////////////////////////////////////// Reg16 ////////////////////////////////////////////////////////////////
void Reg16::operator=(unsigned short v16) {
    *lsb = (unsigned char)v16;     // Assign the value to the register pointed to by lsb
    *msb = (unsigned char)(v16 >> 8);     // Assign the value to the register pointed to by msb
}

Reg16::operator unsigned short() {
    return (unsigned short)((*msb << 8) | *lsb);       //Ou avec les bits de poids faible et de poids fort
}

Reg16::~Reg16(){}

////////////////////////////////////////////////////// 7seg //////////////////////////////////////////////////////////////////
int aff7seg::char2int(char data) {
    int b = data + 0;
    return b;
}

// Affichage d'un nombre entier
void aff7seg::affich(char data, bool hex) {
    int value = char2int(data);
    if (hex) {
        (*brd)[MX3ADDR_7SEG_HEX_L] = (unsigned char)(value & 0xFF);
        (*brd)[MX3ADDR_7SEG_HEX_H] = (unsigned char)((value >> 8) & 0xFF);
    } else {
        (*brd)[MX3ADDR_7SEG_DEC_L] = (unsigned char)(value & 0xFF);
        (*brd)[MX3ADDR_7SEG_DEC_H] = (unsigned char)((value >> 8) & 0xFF);
    }
}
 
aff7seg::~aff7seg(){}

/////////////////////////////////////////////////////////// LEDs ////////////////////////////////////////////////////////////////
void LEDs::operator=(MX3board* sw) {
    brd[MX3ADDR_LED] = sw[MX3ADDR_SW];  //écrit la valeur du switch sur le registre LED
}

LEDs::~LEDs(){}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LCD ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor for the LCD class
LCD::LCD() {
    std::fill(std::begin(line1), std::end(line1), ' '); // Initialize line1 with spaces
    std::fill(std::begin(line2), std::end(line2), ' '); // Initialize line2 with spaces
}

LCD::~LCD() {}

// Write a message to the LCD buffer
void LCD::writeLCD(const std::string& message) {
    int pos = 0;
    for (char c : message) {
        if (pos < 16) {
            line2[pos] = c; // Write to the second line
        } else {
            // Shift line2 to line1 and reset line2
            for (int i = 0; i < 16; i++) {
                line1[i] = line2[i];
                line2[i] = ' ';
            }
            line2[0] = c; // Start a new line
            pos = 0;
        }
        pos++;
    }
}

// Move the second line to the first line
void LCD::next() {
    for (int i = 0; i < 16; i++) {
        line1[i] = line2[i];
    }
    std::fill(std::begin(line2), std::end(line2), ' '); // Clear the second line
}

// Display the current buffer on the LCD
void LCD::display(MX3board& board) {   
    unsigned char gen[35];
    gen[0] = MX3CMD_WR_MAP;       // Command to write a map
    gen[1] = MX3ADDR_LCD_START;   // Start address for the LCD
    gen[2] = 32;                  // Number of bytes to write
    for (int i = 0; i < 16; i++) {
        gen[3 + i] = line1[i];    // Fill line1 data
        gen[19 + i] = line2[i];   // Fill line2 data
    }
    write(board.tty, gen, 35);    // Send the data to the board
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Aux ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor for the Aux class
Aux::Aux(MX3board* brd) {
    board = brd; // Associate the Aux component with the board
}

// Destructor for the Aux class
Aux::~Aux() {}

// Enable or disable the sound
void Aux::enable(bool state) {
    if (state)
        (*board)[MX3ADDR_STATUS] = MX3BIT_STATUS_AUD_EN | (*board)[MX3ADDR_STATUS];
    else
        (*board)[MX3ADDR_STATUS] = ~MX3BIT_STATUS_AUD_EN & (*board)[MX3ADDR_STATUS];
}

// Set the sound frequency (high or low)
void Aux::setFreq(bool high) {
    if (high)
        (*board)[MX3ADDR_STATUS] = MX3BIT_STATUS_AUD_FREQ | (*board)[MX3ADDR_STATUS];
    else
        (*board)[MX3ADDR_STATUS] = ~MX3BIT_STATUS_AUD_FREQ & (*board)[MX3ADDR_STATUS];
}

// Get the number of samples in the FIFO buffer
uint16_t Aux::samplesInFIFO() {
    uint8_t low = (*board)[MX3ADDR_SND_ELTS_L];  // Low byte
    uint8_t high = (*board)[MX3ADDR_SND_ELTS_H]; // High byte
    return (high << 8) | low;                    // Combine high and low bytes
}

// Read a single audio sample from a file
uint8_t Aux::writeSample(std::ifstream& file) {  
    uint8_t sample;
    file.read(reinterpret_cast<char*>(&sample), sizeof(sample)); // Read a byte from the file
    return sample;
}

// Write multiple audio samples to the FIFO buffer
void Aux::writeFIFO(uint16_t size, std::ifstream& file){
    uint16_t cnt{0};
    
    unsigned char gen[255];
    gen[0] = MX3CMD_WR_FF;       // Command to write to FIFO
    gen[1] = MX3ADDR_SND_WFIFO;  // FIFO address
    gen[2] = 252;                // Number of bytes to write

    while(size >= cnt){
        for(int i{3}; i<255; i++){
            if(file.eof()){      // Stop if end of file is reached
                return;
            }
            gen[i] = writeSample(file); // Write a sample to the buffer
        }

        write(board->tty, gen, 255); // Send the buffer to the board
        cnt += 255;
    }
}

// Overload operator<< to load and play an audio file
Aux& Aux::operator<<(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary); // Open the file in binary mode

    if (!file.is_open()) {
        std::cout << "Erreur : Impossible d'ouvrir le fichier audio" << std::endl;
    }

    writeFIFO(64000, file); // Write the initial samples to the FIFO
    enable(true);           // Enable sound
    while(!file.eof()){
        if(samplesInFIFO() <= 32000){ // Refill the FIFO if it gets low
            writeFIFO(32000, file);
        }
    }
    while(samplesInFIFO() > 0); // Wait until the FIFO is empty
    enable(false);              // Disable sound at the end
    return *this;
}

// Overload operator<< to load and play an audio file (const char* version)
Aux& Aux::operator<<(const char* filename) {
    return *this << std::string(filename);
}
