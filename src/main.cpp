#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include "mx3_board.hpp"

namespace fs = std::filesystem;

std::vector<std::string> getMusicFiles(const std::string& folder) {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

std::atomic<bool> playMusic(false); // Variable atomique pour gérer l'état de la musique
std::thread musicThread;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "No device name provided\n";
        return -1;
    }
    
    MX3board brd{argv[1]};
    brd << "j'envoie la fume";
    brd.getSound() << "./song/smoke.txt";

    std::vector<std::string> songs = getMusicFiles("./song");

    if (songs.empty()) {
        std::cerr << "Aucun fichier audio trouvé !\n";
        return -1;
    }

    size_t currentIndex = 0;

    while (true) {
        uint8_t switchState = brd[MX3ADDR_SW];
        
        // Navigation vers le bas (Switch 0 activé)
        if (switchState & 0x01) {  
            currentIndex = (currentIndex + 1) % songs.size();  // Passe au fichier suivant
            brd << songs[currentIndex].substr(7);  // Affiche le nom sans "./song/"
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Attend 1 seconde pour éviter de changer trop rapidement
        }

        // Navigation vers le haut (Switch 2 activé)
        if (switchState & 0x04) {  
            currentIndex = (currentIndex == 0) ? songs.size() - 1 : currentIndex - 1;  // Passe au fichier précédent
            brd << songs[currentIndex].substr(7);  // Affiche le nom sans "./song/"
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Attend 1 seconde pour éviter de changer trop rapidement
        }

    //     // Lecture de la musique (Switch 1 activé)
    //     if (switchState & 0x02 && !playMusic) {  // Si Switch 1 est activé et la musique n'est pas déjà en lecture
    //         playMusic = true;
    //         brd.getSound().play(songs[currentIndex]);  // Lance la musique
    //     } 
    //     // Arrêt de la musique (Switch 1 désactivé)
    //     else if (!(switchState & 0x02) && playMusic) {  // Si Switch 1 est désactivé et la musique est en lecture
    //         playMusic = false;
    //         brd.getSound().stop();  // Arrête la musique
    //     }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Petit délai pour éviter un trop grand nombre de checks
    }

    return 0;
}
