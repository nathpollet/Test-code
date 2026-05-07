#include "SouthBiome.hpp"
SouthBiome::SouthBiome() {
    // Constructeur par défaut
}   
char SouthBiome::generateTile(int tx, int ty) const {
    // Génère une tuile en fonction des coordonnées
    // Par exemple, on peut faire du simple : herbe partout
    return '@';
}