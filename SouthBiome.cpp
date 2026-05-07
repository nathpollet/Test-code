#include "SouthBiome.hpp"
#include <cmath>
SouthBiome::SouthBiome() {
    // Constructeur par défaut
}   
int pseudoRandomSouth(int x, int y) {
    int n = x*135486132 + y*3455487; // Mélange les coordonnées
    n = (n ^ (n >> 11)) * 321451215; // Applique une fonction de mélange
    return abs(n);
}
char SouthBiome::generateTile(int tx, int ty) const {

    int value = pseudoRandomSouth(tx, ty) % 100;

    if (value < 70)
        return 's';   // sable

    if (value < 82)
        return 'C';   // cactus

    if (value < 90)
        return 'r';   // roche

    if (value < 97)
        return 'o';   // oasis

    return 'A';       // ancienne structure
}
