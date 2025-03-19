#ifndef MX3BOARD_HPP
#define MX3BOARD_HPP

#include "mx3board_regs.h"
#include "common_mx3.hpp"
#include <stdexcept>
#include <sstream>
#include <string>

class Reg8;
class LCD;

class MX3board {
private:
    friend class Reg8;
    friend class LCD;

    int tty;
    LCD* lcd;
public:
    MX3board(const char* filename);
    ~MX3board();

    Reg8 operator[](unsigned char addr);

    // Méthode pour manipuler les registres LCD
    void writeLCD(unsigned char addr, char value);
    // Surcharge de l'opérateur << pour écrire sur l'écran LCD via la classe LCD
    MX3board& operator<<(const std::string& message);
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

class LCD 
{
private:
    char line1[16];
    char line2[16];

public:
    LCD();
    void write(const std::string& message);
    void next();

    // Affichage du message sur le LCD via les registres
    void display(MX3board& board);
};

#endif // MX3BOARD_HPP
