#include "WestBiome.hpp"
#include <cmath>

WestBiome::WestBiome() {
    // rien a preparer pour l'instant
}

int pseudoRandomWest(int x, int y) {
    // petit hasard fixe avec les coordonnees
    int n = x * 8975312164 + y * 965725412542;
    n = (n ^ (n >> 17)) * 46485112487;
    return abs(n);
}

char WestBiome::getGroundTile() const {
    return 'G';
}

char WestBiome::generateOverlay(int tx, int ty) const {
    int value = pseudoRandomWest(tx, ty) % 100;

    if (value < 85)
        return '\0';

    if (value < 92)
        return 'R';   // montagne

    if (value < 96)
        return 'm';   // minerai

    if (value < 97)
        return 'L';   // lave

    return 'W';       // ruines / forteresse
}
