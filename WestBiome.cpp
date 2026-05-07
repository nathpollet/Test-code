#include "WestBiome.hpp"
#include <cmath>

WestBiome::WestBiome() {
    // Constructeur par défaut
}
int pseudoRandomWest(int x, int y) {
    // Génère un nombre pseudo-aléatoire basé sur les coordonnées
    int n = x*8975312164 + y*965725412542;
    n = (n ^ (n >> 17)) * 46485112487;
    return abs(n);
}

char WestBiome::generateTile(int tx, int ty) const {

    int value = pseudoRandomWest(tx, ty) % 100;

    if (value < 70)
        return 'r';   // roche

    if (value < 85)
        return 'R';   // montagne

    if (value < 94)
        return 'm';   // minerai

    if (value < 96)
        return 'L';   // lave

    return 'W';       // ruines / forteresse
}

