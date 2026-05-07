#pragma once
#include <SFML/Graphics.hpp>
#include "Biome.hpp"
#include "NorthBiome.hpp"
#include "SouthBiome.hpp"
#include "EastBiome.hpp"
#include "WestBiome.hpp"

class World {
public:
    World(float tileSize, int chunkSize);
    ~World();
    char getTileAt(int tx, int ty) const;
    void draw(sf::RenderWindow& window, sf::Vector2f playerPos);
    sf::Texture tilesetTexture;
private:
    float tileSize;
    int chunkSize;
    int center;  // = chunkSize / 2
    Biome* northBiome;
    Biome* southBiome;
    Biome* eastBiome;
    Biome* westBiome;

    bool isInsideInitialChunk(int tx, int ty) const;
};
