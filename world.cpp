#include "world.hpp"
#include <cmath>
#include <iostream>

namespace {
// pas d'objet sur cette case
constexpr char NoOverlay = '\0';

bool isInitialDoor(int tx, int ty, int chunkSize) {
    // les quatre portes du chunk de depart
    const int middle = chunkSize / 2;
    return (ty == -1 && tx == middle) ||
           (ty == chunkSize && tx == middle) ||
           (tx == -1 && ty == middle) ||
           (tx == chunkSize && ty == middle);
}

bool isInitialDoorExit(int tx, int ty, int chunkSize) {
    // juste apres une porte, on garde le chemin clean
    const int middle = chunkSize / 2;
    return (ty == -2 && tx == middle) ||
           (ty == chunkSize + 1 && tx == middle) ||
           (tx == -2 && ty == middle) ||
           (tx == chunkSize + 1 && ty == middle);
}

bool isInitialWall(int tx, int ty, int chunkSize) {
    // les murs autour du chunk de depart
    return (ty == -1 && tx >= -1 && tx <= chunkSize) ||
           (ty == chunkSize && tx >= -1 && tx <= chunkSize) ||
           (tx == -1 && ty >= -1 && ty <= chunkSize) ||
           (tx == chunkSize && ty >= -1 && ty <= chunkSize);
}

bool rectanglesOverlap(
    float leftA,
    float topA,
    float rightA,
    float bottomA,
    float leftB,
    float topB,
    float rightB,
    float bottomB
) {
    // test simple entre deux rectangles
    return leftA < rightB && rightA > leftB && topA < bottomB && bottomA > topB;
}

bool hasFullTileOverlayHitbox(char overlayTile) {
    // les gros batiments bloquent toute la case
    return overlayTile == 'y' ||
           overlayTile == 'Y' ||
           overlayTile == 'H' ||
           overlayTile == 'A' ||
           overlayTile == 'W';
}

sf::IntRect getTileRect(int col, int row) {
    // la tilesheet a des sprites de 64px avec 32px de marge
    const int margin = 32;
    const int spriteSize = 64;
    const int step = spriteSize + margin;

    return sf::IntRect(
        sf::Vector2i(margin + col * step, margin + row * step),
        sf::Vector2i(spriteSize, spriteSize)
    );
}

void drawTileSprite(sf::RenderWindow& window, const sf::Texture& texture, int col, int row, float tileSize, int tx, int ty) {
    sf::Sprite sprite(texture);
    sprite.setTextureRect(getTileRect(col, row));
    sprite.setScale(sf::Vector2f(
        tileSize / 64.f,
        tileSize / 64.f
    ));
    sprite.setPosition(sf::Vector2f(
        tx * tileSize,
        ty * tileSize
    ));
    window.draw(sprite);
}

void drawGroundTile(sf::RenderWindow& window, const sf::Texture& texture, char tileType, float tileSize, int tx, int ty) {
    // on dessine le sol en premier
    if (tileType == '#') {
        drawTileSprite(window, texture, 0, 0, tileSize, tx, ty);
        drawTileSprite(window, texture, 17, 3, tileSize, tx, ty);
    } else if (tileType == '.') {
        drawTileSprite(window, texture, 0, 0, tileSize, tx, ty);
    } else if (tileType == '^') {
        drawTileSprite(window, texture, 1, 2, tileSize, tx, ty);
    } else if (tileType == 's') {
        drawTileSprite(window, texture, 2, 0, tileSize, tx, ty);
    } else if (tileType == 'G') {
        drawTileSprite(window, texture, 0, 1, tileSize, tx, ty);
    } else {
        drawTileSprite(window, texture, 0, 0, tileSize, tx, ty);
    }
}

void drawOverlayTile(sf::RenderWindow& window, const sf::Texture& texture, char tileType, float tileSize, int tx, int ty) {
    // puis ce qui est pose dessus
    if (tileType == 'T') {
        drawTileSprite(window, texture, 1, 4, tileSize, tx, ty);
    } else if (tileType == 'n') {
        drawTileSprite(window, texture, 7, 3, tileSize, tx, ty);
    } else if (tileType == 'i') {
        drawTileSprite(window, texture, 8, 4, tileSize, tx, ty);
    } else if (tileType == 'y') {
        drawTileSprite(window, texture, 16, 0, tileSize, tx, ty);
    } else if (tileType == 'Y') {
        drawTileSprite(window, texture, 16, 1, tileSize, tx, ty);
    } else if (tileType == 'R') {
        drawTileSprite(window, texture, 5, 4, tileSize, tx, ty);
    } else if (tileType == '~') {
        drawTileSprite(window, texture, 10, 5, tileSize, tx, ty);
    } else if (tileType == 'C') {
        drawTileSprite(window, texture, 10, 4, tileSize, tx, ty);
    } else if (tileType == 'r') {
        drawTileSprite(window, texture, 4, 4, tileSize, tx, ty);
    } else if (tileType == 'o') {
        drawTileSprite(window, texture, 10, 5, tileSize, tx, ty);
    } else if (tileType == 'A') {
        drawTileSprite(window, texture, 7, 6, tileSize, tx, ty);
    } else if (tileType == 'F') {
        drawTileSprite(window, texture, 10, 5, tileSize, tx, ty);
    } else if (tileType == 'H') {
        drawTileSprite(window, texture, 5, 6, tileSize, tx, ty);
    } else if (tileType == 'm') {
        drawTileSprite(window, texture, 8, 5, tileSize, tx, ty);
    } else if (tileType == 'L') {
        drawTileSprite(window, texture, 17, 6, tileSize, tx, ty);
    } else if (tileType == 'W') {
        drawTileSprite(window, texture, 17, 1, tileSize, tx, ty);
    } else if (tileType == '&') {
        drawTileSprite(window, texture, 17, 6, tileSize, tx, ty);
    } else if (tileType == '!') {
        drawTileSprite(window, texture, 10, 4, tileSize, tx, ty);
    } else if (tileType == '@') {
        drawTileSprite(window, texture, 9, 3, tileSize, tx, ty);
    }
}
}

World::World(float tileSize, int chunkSize)
    : tileSize(tileSize), chunkSize(chunkSize)
{
    center = chunkSize / 2;
    northBiome = new NorthBiome();
    southBiome = new SouthBiome();
    eastBiome = new EastBiome();
    westBiome = new WestBiome();
    if (!tilesetTexture.loadFromFile("assets/medieval_tilesheet.png")) {
        std::cerr << "Erreur tilesheet." << std::endl;
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

const Biome* World::getBiomeAt(int tx, int ty) const {
    // on choisit le biome selon la direction depuis le centre
    const int cx = tx - center;
    const int cy = ty - center;

    if (std::abs(cx) > std::abs(cy)) {
        if (cx > 0) {
            return eastBiome;
        }
        return westBiome;
    }

    if (cy > 0) {
        return southBiome;
    }
    return northBiome;
}

char World::getGroundTileAt(int tx, int ty) const {
    // ici on ne renvoie que le sol
    if (isInsideInitialChunk(tx, ty) || isInitialDoor(tx, ty, chunkSize)) {
        return '.';
    }

    if (isInitialWall(tx, ty, chunkSize)) {
        return '#';
    }

    const int cx = tx - center;
    const int cy = ty - center;

    if (cx == cy || cx == -cy) {
        return '#';
    }

    return getBiomeAt(tx, ty)->getGroundTile();
}

char World::getOverlayTileAt(int tx, int ty) const {
    // ici on regarde si un objet doit etre pose sur le sol
    const char groundTile = getGroundTileAt(tx, ty);

    if (groundTile == '#' ||
        isInsideInitialChunk(tx, ty) ||
        isInitialDoor(tx, ty, chunkSize) ||
        isInitialDoorExit(tx, ty, chunkSize)) {
        return NoOverlay;
    }

    const char overlayTile = getBiomeAt(tx, ty)->generateOverlay(tx, ty);

    if (overlayTile == 'Y' && getGroundTileAt(tx, ty - 1) == '#') {
        return NoOverlay;
    }

    if (overlayTile == 'y' && getGroundTileAt(tx, ty + 1) == '#') {
        return NoOverlay;
    }

    return overlayTile;
}

bool World::isWalkableTile(int tx, int ty) const {
    return getGroundTileAt(tx, ty) != '#' && getOverlayTileAt(tx, ty) == NoOverlay;
}

bool World::canMoveTo(float x, float y, float width, float height) const {
    // collision precise du joueur contre les murs et objets
    const float playerLeft = x;
    const float playerTop = y;
    const float playerRight = x + width;
    const float playerBottom = y + height;

    const int leftTile = static_cast<int>(std::floor(playerLeft / tileSize));
    const int rightTile = static_cast<int>(std::floor((playerRight - 0.001f) / tileSize));
    const int topTile = static_cast<int>(std::floor(playerTop / tileSize));
    const int bottomTile = static_cast<int>(std::floor((playerBottom - 0.001f) / tileSize));

    for (int ty = topTile; ty <= bottomTile; ty++) {
        for (int tx = leftTile; tx <= rightTile; tx++) {
            const float tileLeft = tx * tileSize;
            const float tileTop = ty * tileSize;
            const float tileRight = tileLeft + tileSize;
            const float tileBottom = tileTop + tileSize;

            if (getGroundTileAt(tx, ty) == '#' &&
                rectanglesOverlap(playerLeft, playerTop, playerRight, playerBottom, tileLeft, tileTop, tileRight, tileBottom)) {
                return false;
            }

            const char overlayTile = getOverlayTileAt(tx, ty);
            if (overlayTile != NoOverlay) {
                const float overlaySize = hasFullTileOverlayHitbox(overlayTile) ? tileSize : tileSize / 2.f;
                const float overlayInset = (tileSize - overlaySize) / 2.f;
                const float overlayLeft = tileLeft + overlayInset;
                const float overlayTop = tileTop + overlayInset;
                const float overlayRight = overlayLeft + overlaySize;
                const float overlayBottom = overlayTop + overlaySize;

                if (rectanglesOverlap(playerLeft, playerTop, playerRight, playerBottom, overlayLeft, overlayTop, overlayRight, overlayBottom)) {
                    return false;
                }
            }
        }
    }

    return true;
}

char World::getTileAt(int tx, int ty) const {
    const char groundTile = getGroundTileAt(tx, ty);
    if (groundTile == '#') {
        return '#';
    }

    const char overlayTile = getOverlayTileAt(tx, ty);
    if (overlayTile != NoOverlay) {
        return overlayTile;
    }

    return groundTile;
}

void World::draw(sf::RenderWindow& window, sf::Vector2f playerPos) {
    // on ne dessine que les tiles autour du joueur
    const int drawRadius = 12;
    const int playerTileX = static_cast<int>(playerPos.x / tileSize);
    const int playerTileY = static_cast<int>(playerPos.y / tileSize);

    for (int ty = playerTileY - drawRadius; ty <= playerTileY + drawRadius; ty++) {
        for (int tx = playerTileX - drawRadius; tx <= playerTileX + drawRadius; tx++) {
            const char groundTile = getGroundTileAt(tx, ty);
            drawGroundTile(window, tilesetTexture, groundTile, tileSize, tx, ty);

            const char overlayTile = getOverlayTileAt(tx, ty);
            if (overlayTile != NoOverlay) {
                drawOverlayTile(window, tilesetTexture, overlayTile, tileSize, tx, ty);
            }
        }
    }
}
