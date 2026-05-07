#include "NorthBiome.hpp"
#include <cmath>
NorthBiome::NorthBiome() {
    // Constructeur par défaut
}
int pseudoRandomNorth(int x, int y) {
    int n = x*3208481242 + y*2654435761; // Mélange les coordonnées
    n = (n ^ (n >> 13)) * 35412687; // Applique une fonction de mélange
    return abs(n);
}
char NorthBiome::generateTile(int tx, int ty) const {
    int value = pseudoRandomNorth(tx, ty)%100;
    if(value < 70)
        return '^';
    if (value <85)
        return 'T';
    if (value < 95)
        return 'R';
    
    return '~';
}
