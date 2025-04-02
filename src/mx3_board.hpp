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
class Bit;
class Reg16;
class 7seg;
class LEDs;

////////////////////////////////////////////////////// MX3board ///////////////////////////////////////////////////////
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
    friend class LEDs;
};

/////////////////////////////////////////////////////// Reg8 /////////////////////////////////////////////////////////
class Reg8 {
    private:
    MX3board* board;
    unsigned char addr;

public:
    friend class Reg16;

    Reg8(MX3board* card, unsigned char add){
        this->board = card;
        this->addr = add;
    }

    Reg8() {}        //Constructeur sans argument
    void init(MX3board* card, unsigned char add); 

    void operator=(unsigned char value);
    operator unsigned char() const;

    Reg16 r16(*this, Reg8(board, addr + 1));        //Retourne un r16 associé à cette adresse

    Bit operator[](int a);      //Opérateur [] pour renvoyer un Bit
    Bit bit(int pos);      //Retourne un bit précis d'un registre
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
    Aux& operator<<(const char* filename);
};
    

/////////////////////////////////////////////////////// Bit ///////////////////////////////////////////////////////////
class Bit {
    private :
        Reg8& reg;               //registre souhaité
        unsigned char bit_pos;   //position du bit

    public :
        Bit(Reg8& r, unsigned char pos){        //Constructeur
            this -> reg = r;
            this -> bit_pos = pos;
        }; 
        ~Bit();             //Destructeur

        void operator=(int pos);    //modifie la valeur d'un bit
        void operator bool() const;     //Renvoie un bit 
};

//////////////////////////////////////////////////////// Reg16 ///////////////////////////////////////////////////////
//Représente un registre 16 bits, constitué de deux registres 8 bits
class Reg16 {
    private :
        Reg8& lsb;       //Second registre de 8 bits
        Reg8& msb;       //Premier registre de 8 bits

    public:
        Reg16(Reg8& l, Reg8& m){        //Constructeur
            this -> lsb = l;
            this -> msb = m;
        };
        ~Reg16();       //Destructeur

        Reg16();       //Constructeur sans argument

        void operator=(unsigned short v16);       //Pour écrire un entier 16 bits (type short) dans un registre
        operator unsigned short() const;        //Pour lire un registre 16 bits
}

//////////////////////////////////////////////////////// 7seg /////////////////////////////////////////////////////////
class 7seg {
    private :
        MX3board* brd;  // pour les registres pour décimal ou hexadécimal ou symbole

        int char2int(char data);    //fonction conversion 
    
    public :
        7seg(MX3board* board) {
            this -> brd = board;
        }

        void affich(int value, bool hex = false);       // Affiche un nombre entier décimal ou hexadécimal
}

///////////////////////////////////////////////////// LED /////////////////////////////////////////////////////////////
class LEDs {
    private:
        MX3board* brd;
    
    public:
        unsigned char reg_led;
    public:
        LEDs(MX3board* board) {
            this -> brd = board;
        }
        ~LEDs();
     
        void operator=(MX3board* sw);
}
    

#endif // MX3BOARD_HPP
