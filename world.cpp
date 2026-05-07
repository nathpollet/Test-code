#include "world.hpp"

World::World(float tileSize, int chunkSize)
    : tileSize(tileSize), chunkSize(chunkSize)
{
    center = chunkSize / 2;
    northBiome = new NorthBiome();
    southBiome = new SouthBiome();
    eastBiome = new EastBiome();
    westBiome = new WestBiome();
}

World::~World() {
    delete northBiome;
    delete southBiome;
    delete eastBiome;
    delete westBiome;
}

bool World::isInsideInitialChunk(int tx, int ty) const {

    return tx >= 0 && tx < chunkSize && ty >= 0 && ty < chunkSize;  
}

char World::getTileAt(int tx, int ty) const {

    // 1. Si dans chunk initial → herbe
    // if ( ... ) return '.';   // à implémenter
    if (isInsideInitialChunk(tx, ty)) {
        return '.';
    }
// Mur haut
if (ty == -1 && tx >= -1 && tx <= chunkSize)
    return '#';

// Mur bas
if (ty == chunkSize && tx >= -1 && tx <= chunkSize)
    return '#';

// Mur gauche
if (tx == -1 && ty >= -1 && ty <= chunkSize)
    return '#';

// Mur droite
if (tx == chunkSize && ty >= -1 && ty <= chunkSize)
    return '#';

    // 2. Convertis coordonnées en coordonnées centrées
    // cx = tx - center;
    // cy = ty - center;
const int cx = tx - center;
const int cy = ty - center;
    // 3. Première diagonale (↘ ↖)
    // if (cx == cy) return '#';
if (cx == cy) {
    return '#';
}
    // 4. Deuxième diagonale (↗ ↙)
    // if (cx == -cy) return '#';
if (cx == -cy) {
    return '#';
}
if (std::abs(cx) > std::abs(cy)) {
    if (cx > 0) {
        return eastBiome->generateTile(tx, ty);
    } else {
        return westBiome->generateTile(tx, ty);
    }
} else {
    if (cy > 0) {
        return southBiome->generateTile(tx, ty);
    } else {
        return northBiome->generateTile(tx, ty);
    }

}
const Biome* biome = nullptr;
    if (cy < 0) {
        biome = northBiome;
    } else if (cy > 0) {
        biome = southBiome;
    } else if (cx < 0) {
        biome = westBiome;
    } else {
        biome = eastBiome;
    }
    return biome->generateTile(tx, ty);
}

void World::draw(sf::RenderWindow& window, sf::Vector2f playerPos) {
    // À implémenter :
    // Boucler sur une zone autour du joueur
    // Pour chaque tile, appeler getTileAt()
    // Dessiner un RectangleShape de tileSize
    const int drawRadius = 25; // Nombre de tiles à dessiner autour du joueur
    const int playerTileX = static_cast<int>(playerPos.x / tileSize);
    const int playerTileY = static_cast<int>(playerPos.y / tileSize);
    for (int ty = playerTileY - drawRadius; ty <= playerTileY + drawRadius; ty++) {
        for (int tx = playerTileX - drawRadius; tx <= playerTileX + drawRadius; tx++) {
            char tileType = getTileAt(tx, ty);
            sf::RectangleShape tileShape({tileSize, tileSize});
            tileShape.setPosition(sf::Vector2f(tx * tileSize, ty * tileSize));
            if (tileType == '#') {
                tileShape.setFillColor(sf::Color::Black);
            } 
            else if (tileType == '.') {
                tileShape.setFillColor(sf::Color::Green);
            }
            else if (tileType == '^') {
                tileShape.setFillColor(sf::Color::White);
            }
            else if (tileType == '&') {
                tileShape.setFillColor(sf::Color::Red);
            }
            else if (tileType == '!') {
                tileShape.setFillColor(sf::Color::Green);
        
            }
            else if (tileType == '@') {
                tileShape.setFillColor(sf::Color::Yellow);
            }
            window.draw(tileShape);
        }
    }
}
