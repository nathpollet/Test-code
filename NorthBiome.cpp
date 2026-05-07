#include "NorthBiome.hpp"
NorthBiome::NorthBiome() {
    // Constructeur par défaut
}
char NorthBiome::generateTile(int tx, int ty) const {
    // Génère une tuile en fonction des coordonnées
    // Par exemple, on peut faire du simple : herbe partout
    return '^';
}