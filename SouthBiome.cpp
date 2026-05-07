#include "SouthBiome.hpp"
#include <cmath>

SouthBiome::SouthBiome() {
    // rien a preparer pour l'instant
}

int pseudoRandomSouth(int x, int y) {
    // petit hasard fixe avec les coordonnees
    int n = x * 135486132 + y * 3455487;
    n = (n ^ (n >> 11)) * 321451215;
    return abs(n);
}

char SouthBiome::getGroundTile() const {
    return 's';
}

char SouthBiome::generateOverlay(int tx, int ty) const {
    int value = pseudoRandomSouth(tx, ty) % 100;

    if (value < 85)
        return '\0';

    if (value < 91)
        return 'C';   // cactus

    if (value < 95)
        return 'r';   // roche

    if (value < 98)
        return 'C';   // oasis / vegetation

    return 'A';       // ancienne structure
}
