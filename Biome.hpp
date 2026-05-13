#pragma once

/**
 * Base commune de tous les biomes: chaque biome dit quel sol il utilise
 * et quels petits objets il balance dessus.
 */
class Biome {
public:
    /**
     * Destructeur virtuel, comme ca on peut supprimer un biome via un pointeur
     * vers Biome sans faire de betise.
     */
    virtual ~Biome() = default;

    /**
     * Renvoie la tuile de sol de base du biome.
     */
    virtual char getGroundTile() const = 0;

    /**
     * Decide ce qu'on pose sur la case, ou rien du tout si la case reste clean.
     */
    virtual char generateOverlay(int tx, int ty) const = 0;
};

/**
 * Biome du nord: ambiance montagne/neige avec des chateaux poses rarement.
 */
class NorthBiome : public Biome {
public:
    NorthBiome();

    char getGroundTile() const override;
    char generateOverlay(int tx, int ty) const override;
};

/**
 * Biome du sud: coin desertique avec cactus, roches et vieilles structures.
 */
class SouthBiome : public Biome {
public:
    SouthBiome();

    char getGroundTile() const override;
    char generateOverlay(int tx, int ty) const override;
};

/**
 * Biome de l'est: zone plutot verte/classique avec quelques arbres et maisons.
 */
class EastBiome : public Biome {
public:
    EastBiome();

    char getGroundTile() const override;
    char generateOverlay(int tx, int ty) const override;
};

/**
 * Biome de l'ouest: terrain plus rude, avec montagnes, minerai et ruines.
 */
class WestBiome : public Biome {
public:
    WestBiome();

    char getGroundTile() const override;
    char generateOverlay(int tx, int ty) const override;
};
