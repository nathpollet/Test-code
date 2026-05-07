#include "EastBiome.hpp"
#include <cmath>

EastBiome::EastBiome() {
    // rien a preparer pour l'instant
}

int pseudoRandomEast(int x, int y) {
    // petit hasard fixe avec les coordonnees
    int n = x * 146474451 + y * 566746813;
    n = (n ^ (n >> 11)) * 468413125;
    return abs(n);
}

char EastBiome::getGroundTile() const {
    return '.';
}

char EastBiome::generateOverlay(int tx, int ty) const {
    int value = pseudoRandomEast(tx, ty) % 100;

    if (value < 87)
        return '\0';

    if (value < 94)
        return 'T';

    if (value < 98)
        return 'F';

    if (value < 99)
        return '\0';

    return 'H';
}
