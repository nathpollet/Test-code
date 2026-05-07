#include "NorthBiome.hpp"
#include <cmath>

NorthBiome::NorthBiome() {
    // rien a preparer pour l'instant
}

int pseudoRandomNorth(int x, int y) {
    // petit hasard fixe avec les coordonnees
    int n = x * 3208481242 + y * 2654435761;
    n = (n ^ (n >> 13)) * 35412687;
    return abs(n);
}

bool shouldPlaceNorthCastleBase(int x, int y) {
    // chateau rare, histoire que ca reste special
    return pseudoRandomNorth(x, y) % 1000 < 4;
}

char NorthBiome::getGroundTile() const {
    return '^';
}

char NorthBiome::generateOverlay(int tx, int ty) const {
    // y = haut du chateau, y majuscule = bas
    if (shouldPlaceNorthCastleBase(tx, ty + 1))
        return 'y';

    if (shouldPlaceNorthCastleBase(tx, ty))
        return 'Y';

    int value = pseudoRandomNorth(tx, ty) % 100;

    if (value < 85)
        return '\0';

    if (value < 92)
        return 'n';

    if (value < 97)
        return 'i';

    return 'n';
}
