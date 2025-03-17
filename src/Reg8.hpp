#ifndef REG8_HPP
#define REG8_HPP

#include "mx3_board.hpp"

class Reg8
{
private:
    MX3board * board;
    unsigned char addr;
public:
    Reg8(MX3board * card, unsigned char add);
    ~Reg8() = default;
    
    Reg8& operator=(unsigned char value);
    operator unsigned char() const;
};

Reg8::Reg8(MX3board * card, unsigned char add){ 
    board = card;
    addr = add;
}

Reg8& Reg8::operator=(unsigned char value) {
    (*board)[addr] = value;  // On accède à l'adresse 'addr' via l'opérateur []
    return *this; // Retourne l'objet actuel pour permettre l'enchaînement des affectations
}

Reg8::operator unsigned char() const {
    return (*board)[addr];  // Utilisation de l'opérateur [] pour lire la valeur à l'adresse
}

#endif