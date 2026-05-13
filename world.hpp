#pragma once
#include <SFML/Graphics.hpp>
#include "Biome.hpp"

/**
 * Gere la carte du jeu: choix des biomes, collisions et dessin des tuiles.
 */
class World {
public:
    /**
     * Cree le monde avec une taille de tuile et une taille de chunk de depart.
     */
    World(float tileSize, int chunkSize);

    /**
     * Nettoie les biomes crees a la main.
     */
    ~World();

    /**
     * Renvoie la tuile visible finale a une position donnee.
     */
    char getTileAt(int tx, int ty) const;

    /**
     * Renvoie uniquement la tuile de sol, sans les objets poses dessus.
     */
    char getGroundTileAt(int tx, int ty) const;

    /**
     * Renvoie l'objet pose sur une case, ou rien si la case est vide.
     */
    char getOverlayTileAt(int tx, int ty) const;

    /**
     * Dit si une case est praticable pour le joueur.
     */
    bool isWalkableTile(int tx, int ty) const;

    /**
     * Verifie si la hitbox du joueur peut aller a cette position.
     */
    bool canMoveTo(float x, float y, float width, float height) const;

    /**
     * Dessine les tuiles proches du joueur pour eviter de tout afficher.
     */
    void draw(sf::RenderWindow& window, sf::Vector2f playerPos);

    /**
     * Texture commune de la tilesheet medievale.
     */
    sf::Texture tilesetTexture;
private:
    /**
     * Taille d'une tuile en pixels.
     */
    float tileSize;

    /**
     * Taille du chunk de depart, en nombre de tuiles.
     */
    int chunkSize;

    /**
     * Centre du chunk, pratique pour savoir vers quel biome partir.
     */
    int center;  // = chunkSize / 2

    /**
     * Biomes utilises autour du chunk de depart.
     */
    Biome* northBiome;
    Biome* southBiome;
    Biome* eastBiome;
    Biome* westBiome;

    /**
     * Dit si une coordonnee tombe dans le chunk de depart.
     */
    bool isInsideInitialChunk(int tx, int ty) const;

    /**
     * Chope le biome correspondant a une position autour du centre.
     */
    const Biome* getBiomeAt(int tx, int ty) const;
};
