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

private:
    float tileSize;
    int chunkSize;
    int center;  // = chunkSize / 2
    Biome* northBiome = new NorthBiome();
    Biome* southBiome = new SouthBiome();
    Biome* eastBiome = new EastBiome();
    Biome* westBiome = new WestBiome();

    bool isInsideInitialChunk(int tx, int ty) const;
};
