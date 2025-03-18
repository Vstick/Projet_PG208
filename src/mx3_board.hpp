#ifndef MX3BOARD_HPP
#define MX3BOARD_HPP

#include "mx3board_regs.h"
#include "common_mx3.hpp"
#include <stdexcept>

class Reg8;

class MX3board {
private:
    int tty;

public:
    MX3board(const char* filename);
    ~MX3board();

    Reg8 operator[](unsigned char addr);

    friend class Reg8;
};

class Reg8 {
    private:
        MX3board* board;
        unsigned char addr;
    
    public:
        Reg8(MX3board* card, unsigned char add){
            this->board = card;
            this->addr = add;
        }
    
        void operator=(unsigned char value);
        operator unsigned char() const;
    };
    

#endif // MX3BOARD_HPP