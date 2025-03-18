#include <iostream>
#include "mx3_board.hpp"   


int main(int argc, char *argv[]){
    if (argc<2) {
        printf("No device name provided\n");
        return -1;
    }

    MX3board board {argv[1]};
    
    return 0;
}