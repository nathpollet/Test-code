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
    /**
     * Cree le biome du nord.
     */
    NorthBiome();

    /**
     * Renvoie la tuile de sol du nord.
     */
    char getGroundTile() const override;

    /**
     * Genere les decors propres au nord a une position donnee.
     */
    char generateOverlay(int tx, int ty) const override;
};

/**
 * Biome du sud: coin desertique avec cactus, roches et vieilles structures.
 */
class SouthBiome : public Biome {
public:
    /**
     * Cree le biome du sud.
     */
    SouthBiome();

    /**
     * Renvoie la tuile de sol du sud.
     */
    char getGroundTile() const override;

    /**
     * Genere les decors propres au sud a une position donnee.
     */
    char generateOverlay(int tx, int ty) const override;
};

/**
 * Biome de l'est: zone plutot verte/classique avec quelques arbres et maisons.
 */
class EastBiome : public Biome {
public:
    /**
     * Cree le biome de l'est.
     */
    EastBiome();

    /**
     * Renvoie la tuile de sol de l'est.
     */
    char getGroundTile() const override;

    /**
     * Genere les decors propres a l'est a une position donnee.
     */
    char generateOverlay(int tx, int ty) const override;
};

/**
 * Biome de l'ouest: terrain plus rude, avec montagnes, minerai et ruines.
 */
class WestBiome : public Biome {
public:
    /**
     * Cree le biome de l'ouest.
     */
    WestBiome();

    /**
     * Renvoie la tuile de sol de l'ouest.
     */
    char getGroundTile() const override;

    /**
     * Genere les decors propres a l'ouest a une position donnee.
     */
    char generateOverlay(int tx, int ty) const override;
};
