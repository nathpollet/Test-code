#include "EastBiome.hpp"

EastBiome::EastBiome() {
    // Constructeur par défaut
}

char EastBiome::generateTile(int tx, int ty) const {
    // Génère une tuile en fonction des coordonnées
    // Par exemple, on peut faire du simple : herbe partout
    return '&';
}