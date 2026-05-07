#include "world.hpp"
#include <iostream>

World::World(float tileSize, int chunkSize)
    : tileSize(tileSize), chunkSize(chunkSize)
{
    center = chunkSize / 2;
    northBiome = new NorthBiome();
    southBiome = new SouthBiome();
    eastBiome = new EastBiome();
    westBiome = new WestBiome();
    if (!tilesetTexture.loadFromFile("assets/medieval_tilesheet.png")) {
    std::cerr << "Erreur : impossible de charger le tilesheet." << std::endl;
}
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
if (ty== -1 && tx == chunkSize/2)
    return '.';
else if (ty == -1 && tx >= -1 && tx <= chunkSize)
    return '#';

// Mur bas
if (ty == chunkSize && tx == chunkSize/2)
    return '.';
else if (ty == chunkSize && tx >= -1 && tx <= chunkSize)
    return '#';

// Mur gauche
if (tx == -1 && ty == chunkSize/2)
    return '.';
else if (tx == -1 && ty >= -1 && ty <= chunkSize)
    return '#';

// Mur droite
if (tx == chunkSize && ty == chunkSize/2)
    return '.';
else if (tx == chunkSize && ty >= -1 && ty <= chunkSize)
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
sf::IntRect getTileRect(int col, int row) {
    const int margin = 32;
    const int step = 64;
    const int spriteSize = 64;

    return sf::IntRect(
        sf::Vector2i(col*margin + col * step, row*margin + row * step),
        sf::Vector2i(spriteSize, spriteSize)
    );
}

void World::draw(sf::RenderWindow& window, sf::Vector2f playerPos) {
    // À implémenter :
    // Boucler sur une zone autour du joueur
    // Pour chaque tile, appeler getTileAt()
    // Dessiner un RectangleShape de tileSize
    const int drawRadius = 12; // Nombre de tiles à dessiner autour du joueur
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
            if (tileType == '.') {
                sf::Sprite sprite(tilesetTexture);

                sprite.setTextureRect(sf::IntRect(
                sf::Vector2i(32, 32),
                sf::Vector2i(96, 96)
                ));

                sprite.setScale(sf::Vector2f(
                    static_cast<float>(tileSize) / 64.f,
                    static_cast<float>(tileSize) / 64.f
                ));

                sprite.setPosition(sf::Vector2f(
                    tx * tileSize,
                    ty * tileSize
                ));
    
            
                window.draw(sprite);
                continue;
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
            else if (tileType == 'T') {
                tileShape.setFillColor(sf::Color(34, 139, 34)); // Vert forêt
            }
            else if (tileType == 'R') {
                tileShape.setFillColor(sf::Color(128, 128, 128)); // Gris roche
            }
            else if (tileType == '~') {
                tileShape.setFillColor(sf::Color(0, 191, 255)); // Bleu eau
            }
            else if (tileType == 's') {
                tileShape.setFillColor(sf::Color(230, 210, 120));
            }

            else if (tileType == 'C') {
                tileShape.setFillColor(sf::Color(20, 150, 20));
                
            }

            else if (tileType == 'r') {
                tileShape.setFillColor(sf::Color(100, 80, 60));
            }   

            else if (tileType == 'o') {
                tileShape.setFillColor(sf::Color(40, 180, 255));
            }

            else if (tileType == 'A') {
                tileShape.setFillColor(sf::Color(180, 140, 70));
            }
            else if (tileType == 'F') {
                tileShape.setFillColor(sf::Color(255, 100, 180));
            }

            else if (tileType == 'H') {
                tileShape.setFillColor(sf::Color(120, 70, 20));
            }
            else if (tileType == 'm') {
                tileShape.setFillColor(sf::Color(100, 220, 255));
            }

            else if (tileType == 'L') {
                tileShape.setFillColor(sf::Color(255, 80, 0));
            }

            else if (tileType == 'W') {
                tileShape.setFillColor(sf::Color(90, 90, 90));
            }



             window.draw(tileShape);
}
           
        }
    }

