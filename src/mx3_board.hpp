#ifndef MX3BOARD_HPP
#define MX3BOARD_HPP

#include "mx3board_regs.h"
#include "common_mx3.hpp"
#include <stdexcept>
#include <cstdint>
#include <sstream>
#include <string>
#include <fstream>

class Reg8;
class LCD;
class Aux;

class MX3board {
private:
    friend class Reg8;
    friend class LCD;
    friend class Aux;

    int tty;
    LCD* lcd;
    Aux* sound; // Ajout de l'objet son

public:
    MX3board(const char* filename);
    ~MX3board();

    Reg8 operator[](unsigned char addr);

    // Méthode pour manipuler les registres externes
    void writeEXT(unsigned char addr, char value);
    
    // Surcharge de l'opérateur << pour écrire sur l'écran LCD
    MX3board& operator<<(const std::string& message);

    // Accès au module audio
    Aux& getSound();
};

class Reg8 {
private:
    MX3board* board;
    unsigned char addr;

public:
    Reg8(MX3board* card, unsigned char add);
    void operator=(unsigned char value);
    operator unsigned char() const;
};

class LCD {
private:
    char line1[16];
    char line2[16];

public:
    LCD();
    void writeLCD(const std::string& message);
    void next();

    // Affichage du message sur le LCD via les registres
    void display(MX3board& board);
};

class Aux {
private:
    MX3board* board;

public:
    Aux(MX3board* brd);
    ~Aux();
    
    void enable(bool state);  // Active ou désactive l'audio
    void setFreq(bool high);  // Définit la fréquence d'échantillonnage

    // Gestion de la FIFO
    uint16_t samplesInFIFO();  
    uint8_t writeSample(std::ifstream& file);
    void writeFIFO(uint16_t size, std::ifstream& file);

    // Lecture d'un fichier audio
    Aux& operator<<(const std::string& filename);
};

#endif // MX3BOARD_HPP
