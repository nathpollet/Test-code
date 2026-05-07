#include "EastBiome.hpp"
#include <cmath>
EastBiome::EastBiome() {
    // Constructeur par défaut
}
int pseudoRandomEast(int x, int y) {
    int n = x*146474451 + y*566746813; // Mélange les coordonnées
    n = (n ^ (n >> 11)) * 468413125; // Applique une fonction de mélange
    return abs(n);
}
char EastBiome::generateTile(int tx, int ty) const {
    int value = pseudoRandomEast(tx, ty)%100;
  
    if (value < 70)
        return '.';   // herbe

    if (value < 85)
        return 'T';   // arbre

    if (value < 90)
        return '~';   // eau

    if (value < 97)
        return 'F';   // fleurs / végétation

    return 'H';       // vieille cabane / structure

}