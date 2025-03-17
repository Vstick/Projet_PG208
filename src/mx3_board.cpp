#include <iostream>
#include <vector>
#include <cmath>

#include "mx3_board.hpp"
#include "main_mx3.c"

//Constructeur
MX3board::MX3board(const char* filename) {
    board_open(char* filename);
}

//Destructeur
MX3board::~MX3board() { // Destructeur
    board_close();
}