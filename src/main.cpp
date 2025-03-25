#include <iostream>
#include <cmath>
#include <chrono>  // Pour std::chrono::milliseconds
#include <thread>
#include "mx3_board.hpp"   

void wait(int milli);

int main(int argc, char *argv[]){
    if (argc<2) {
        printf("No device name provided\n");
        return -1;
    }
    MX3board brd {argv[1]};
    brd[MX3ADDR_LED]= 0x55;
    std::cout << "switch : " << std::hex << brd[MX3ADDR_SW]+0 << std::endl;
    while (1)
    {
        
        brd << "Hello World";
        wait(1000);
        brd << "Je suis heureux d'aller en cours";
        wait(1000);
        // brd << "voici la musique";
        // try
        // {
        //     brd.getSound() << "./song/soupe.txt";
        // }
        // catch(const std::exception& e)
        // {
        //     std::cerr << e.what() << '\n';
        // }
              

        std::cout << "Attente de 10 seconde..." << std::endl;
        wait(10000);
        std::cout << "Reprise du programme." << std::endl;
    }
    return 0;
}

void wait(int milli){
    std::this_thread::sleep_for(std::chrono::milliseconds(milli));
}