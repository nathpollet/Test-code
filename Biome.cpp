#include "Biome.hpp"
#include <cmath>

namespace {
/**
 * Fabrique un hasard stable avec les coordonnees du nord.
 */
int pseudoRandomNorth(int x, int y) {
    int n = x * 3208481242 + y * 2654435761;
    n = (n ^ (n >> 13)) * 35412687;
    return std::abs(n);
}

/**
 * Dit si on pose la base d'un chateau ici, mais seulement de temps en temps.
 */
bool shouldPlaceNorthCastleBase(int x, int y) {
    return pseudoRandomNorth(x, y) % 1000 < 4;
}

/**
 * Fabrique un hasard stable avec les coordonnees du sud.
 */
int pseudoRandomSouth(int x, int y) {
    int n = x * 135486132 + y * 3455487;
    n = (n ^ (n >> 11)) * 321451215;
    return std::abs(n);
}

/**
 * Fabrique un hasard stable avec les coordonnees, histoire que la map reste pareil.
 */
int pseudoRandomEast(int x, int y) {
    int n = x * 146474451 + y * 566746813;
    n = (n ^ (n >> 11)) * 468413125;
    return std::abs(n);
}

/**
 * Fabrique un hasard stable avec les coordonnees de l'ouest.
 */
int pseudoRandomWest(int x, int y) {
    int n = x * 8975312164 + y * 965725412542;
    n = (n ^ (n >> 17)) * 46485112487;
    return std::abs(n);
}
}

/**
 * Cree le biome du nord.
 */
NorthBiome::NorthBiome() {
    // rien a preparer pour l'instant
}

/**
 * Renvoie le sol montagne/neige du biome nord.
 */
char NorthBiome::getGroundTile() const {
    return '^';
}

/**
 * Genere les chateaux et petits decors du biome nord.
 */
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

/**
 * Cree le biome du sud.
 */
SouthBiome::SouthBiome() {
    // rien a preparer pour l'instant
}

/**
 * Renvoie le sol sableux du biome sud.
 */
char SouthBiome::getGroundTile() const {
    return 's';
}

/**
 * Genere les cactus, roches et structures du biome sud.
 */
char SouthBiome::generateOverlay(int tx, int ty) const {
    int value = pseudoRandomSouth(tx, ty) % 100;

    if (value < 85)
        return '\0';

    if (value < 91)
        return 'C';

    if (value < 95)
        return 'r';

    if (value < 98)
        return 'C';

    return 'A';
}

/**
 * Cree le biome de l'est.
 */
EastBiome::EastBiome() {
    // rien a preparer pour l'instant
}

/**
 * Renvoie le sol herbeux du biome est.
 */
char EastBiome::getGroundTile() const {
    return '.';
}

/**
 * Genere les arbres, feux et maisons du biome est.
 */
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

/**
 * Cree le biome de l'ouest.
 */
WestBiome::WestBiome() {
    // rien a preparer pour l'instant
}

/**
 * Renvoie le sol du biome ouest.
 */
char WestBiome::getGroundTile() const {
    return 'G';
}

/**
 * Genere les montagnes, minerais, lave et ruines du biome ouest.
 */
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
