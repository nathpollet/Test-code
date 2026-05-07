#include "WestBiome.hpp"

WestBiome::WestBiome() {
    // Constructeur par défaut
}

char WestBiome::generateTile(int tx, int ty) const {
    // Génère une tuile en fonction des coordonnées
    // Par exemple, on peut faire de la montagne partout
    return '!';
}