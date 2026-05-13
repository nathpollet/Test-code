#include <SFML/Graphics.hpp>
#include "entity.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "world.hpp"
#include "character.hpp"

namespace {
struct Loot {
    sf::Vector2f position;
    std::string item;
    bool ramasse;
};

/**
 * Generation des objets de mainbis, sans remplacer la generation du World.
 * Les murs restent geres par World, donc ici on ne renvoie que les bonus.
 */
char generateCollectibleTile(int worldX, int worldY) {
    if (worldX % 2 != 0 || worldY % 2 != 0)
        return '\0';

    int seed = (worldX * 73856093) ^ (worldY * 19349663);
    int chance = std::abs(seed % 100);

    if (chance < 15)
        return '\0';

    if (chance < 30)
        return 'p'; // piece

    if (chance < 31)
        return 'n'; // nourriture

    if (chance < 35)
        return 'b'; // bandage

    return '\0';
}

bool containsPosition(const std::vector<sf::Vector2i>& positions, int x, int y) {
    for (const auto& pos : positions) {
        if (pos.x == x && pos.y == y) {
            return true;
        }
    }

    return false;
}

void drawCoin(sf::RenderWindow& window, sf::Vector2f center, float radius) {
    sf::CircleShape coin(radius);
    coin.setFillColor(sf::Color(235, 190, 35));
    coin.setOutlineThickness(2.f);
    coin.setOutlineColor(sf::Color(130, 90, 20));
    coin.setPosition({center.x - radius, center.y - radius});
    window.draw(coin);
}

void drawFood(sf::RenderWindow& window, sf::Vector2f center) {
    sf::CircleShape apple(10.f);
    apple.setFillColor(sf::Color(200, 45, 45));
    apple.setOutlineThickness(1.f);
    apple.setOutlineColor(sf::Color(110, 25, 25));
    apple.setPosition({center.x - 10.f, center.y - 6.f});
    window.draw(apple);

    sf::RectangleShape stem({3.f, 7.f});
    stem.setFillColor(sf::Color(95, 55, 25));
    stem.setPosition({center.x - 1.f, center.y - 13.f});
    window.draw(stem);

    sf::CircleShape leaf(4.f);
    leaf.setFillColor(sf::Color(60, 150, 65));
    leaf.setPosition({center.x + 2.f, center.y - 13.f});
    window.draw(leaf);
}

void drawBandage(sf::RenderWindow& window, sf::Vector2f center) {
    sf::RectangleShape bandage({30.f, 16.f});
    bandage.setFillColor(sf::Color(235, 220, 185));
    bandage.setOutlineThickness(1.f);
    bandage.setOutlineColor(sf::Color(120, 95, 70));
    bandage.setPosition({center.x - 15.f, center.y - 8.f});
    window.draw(bandage);

    sf::RectangleShape crossVertical({4.f, 10.f});
    crossVertical.setFillColor(sf::Color(190, 40, 40));
    crossVertical.setPosition({center.x - 2.f, center.y - 5.f});
    window.draw(crossVertical);

    sf::RectangleShape crossHorizontal({10.f, 4.f});
    crossHorizontal.setFillColor(sf::Color(190, 40, 40));
    crossHorizontal.setPosition({center.x - 5.f, center.y - 2.f});
    window.draw(crossHorizontal);
}

void drawLootSign(sf::RenderWindow& window, sf::Vector2f center) {
    sf::RectangleShape sign({22.f, 22.f});
    sign.setFillColor(sf::Color(140, 70, 200));
    sign.setOutlineThickness(2.f);
    sign.setOutlineColor(sf::Color(55, 25, 85));
    sign.setPosition({center.x - 11.f, center.y - 11.f});
    window.draw(sign);

    sf::RectangleShape mark({4.f, 12.f});
    mark.setFillColor(sf::Color::White);
    mark.setPosition({center.x - 2.f, center.y - 8.f});
    window.draw(mark);

    sf::CircleShape dot(2.5f);
    dot.setFillColor(sf::Color::White);
    dot.setPosition({center.x - 2.5f, center.y + 5.f});
    window.draw(dot);
}
}

/**
 * Lance la fenetre, gere les controles du joueur et dessine la scene.
 */
int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    sf::View view(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));
    Player playerObject(50.0f, 50.0f);
    World world(50.f, 11);
    sf::Clock clock;
    float speed = 100.0f;
    const float TILE_SIZE = 50.f;
    const int PICKUP_DRAW_RADIUS = 12;
    const float ENEMY_ACTIVATION_DISTANCE = 4.f * TILE_SIZE;
    const float ENEMY_DESINTEGRATION_DISTANCE = 5.f * TILE_SIZE;
    const float ENEMY_DOOR_DISTANCE = 25.f;
    const int CHUNK_SIZE = 11;
    const int DOOR_TILE = CHUNK_SIZE / 2;

    std::vector<sf::Vector2i> piecesRamassees;
    std::vector<sf::Vector2i> pommesMangees;
    std::vector<sf::Vector2i> bandagesUtilises;
    std::vector<Loot> loots;
    std::vector<Mechant> mechants;
    mechants.push_back(Mechant("Mechant du nord", 1, (DOOR_TILE + 0.5f) * TILE_SIZE, (-1.f - ENEMY_DOOR_DISTANCE + 0.5f) * TILE_SIZE));
    mechants.push_back(Mechant("Mechant du sud", 2, (DOOR_TILE + 0.5f) * TILE_SIZE, (CHUNK_SIZE + ENEMY_DOOR_DISTANCE + 0.5f) * TILE_SIZE));
    mechants.push_back(Mechant("Mechant de l'est", 3, (CHUNK_SIZE + ENEMY_DOOR_DISTANCE + 0.5f) * TILE_SIZE, (DOOR_TILE + 0.5f) * TILE_SIZE));
    mechants.push_back(Mechant("Mechant de l'ouest", 4, (-1.f - ENEMY_DOOR_DISTANCE + 0.5f) * TILE_SIZE, (DOOR_TILE + 0.5f) * TILE_SIZE));

    // taille de la hitbox du joueur
    const float PLAYER_SIZE = 36.f;
    const float PLAYER_HALF_SIZE = PLAYER_SIZE / 2.f;

    // premiere frame du chevalier dans la spritesheet
    const int PLAYER_FRAME_WIDTH = 70;
    const int PLAYER_FRAME_HEIGHT = 90;
    const float PLAYER_SCALE = PLAYER_SIZE / static_cast<float>(PLAYER_FRAME_HEIGHT);
    const float PLAYER_DRAW_WIDTH = PLAYER_FRAME_WIDTH * PLAYER_SCALE;
    const float PLAYER_DRAW_X_OFFSET = (PLAYER_SIZE - PLAYER_DRAW_WIDTH) / 2.f;
    sf::Texture playerTexture;
    bool playerTextureLoaded = playerTexture.loadFromFile("assets/knight.png");
    if (!playerTextureLoaded) {
        std::cerr << "Erreur : impossible de charger le sprite du joueur." << std::endl;
    }

    sf::Font font;
    bool fontLoaded = font.openFromFile("arial.ttf");
    if (!fontLoaded) {
        fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    }
    if (!fontLoaded) {
        std::cerr << "Erreur : impossible de charger la police arial.ttf." << std::endl;
    }

    sf::Text deathText(font, "GAME OVER, LE CHEVALIER EST MORT", 30);
    deathText.setFillColor(sf::Color::Red);
    deathText.setStyle(sf::Text::Bold);
    sf::FloatRect textRect = deathText.getLocalBounds();
    deathText.setOrigin({
        textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f
    });
    deathText.setPosition({400.f, 300.f});

    sf::View uiView(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        if (playerObject.vie > 0) {
            playerObject.updateStats(deltaTime);

            const float currentSpeed = speed * playerObject.getMultiplicateurVitesse();
            float nextY = playerObject.getY();
            float nextX = playerObject.getX();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
                nextY  -= currentSpeed * deltaTime;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                nextY += currentSpeed * deltaTime;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
                nextX -= currentSpeed * deltaTime;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                nextX += currentSpeed * deltaTime;
            }

            // on bouge seulement si la hitbox passe
            if (world.canMoveTo(nextX, nextY, PLAYER_SIZE, PLAYER_SIZE)) {
                playerObject.move(nextX - playerObject.getX(),
                                  nextY - playerObject.getY());
            }

            const int gridX = static_cast<int>(std::floor((playerObject.getX() + PLAYER_HALF_SIZE) / TILE_SIZE));
            const int gridY = static_cast<int>(std::floor((playerObject.getY() + PLAYER_HALF_SIZE) / TILE_SIZE));

            if (world.isWalkableTile(gridX, gridY)) {
                const char collectibleTile = generateCollectibleTile(gridX, gridY);

                if (collectibleTile == 'p' && !containsPosition(piecesRamassees, gridX, gridY)) {
                    playerObject.ajouterArgent(1);
                    piecesRamassees.push_back(sf::Vector2i(gridX, gridY));
                    std::cout << "Piece ramassee ! Nouveau solde : " << playerObject.argent
                              << " | Niveau : " << playerObject.niveau << std::endl;
                } else if (collectibleTile == 'n' && !containsPosition(pommesMangees, gridX, gridY)) {
                    playerObject.manger();
                    pommesMangees.push_back(sf::Vector2i(gridX, gridY));
                } else if (collectibleTile == 'b' && !containsPosition(bandagesUtilises, gridX, gridY)) {
                    playerObject.soigner();
                    bandagesUtilises.push_back(sf::Vector2i(gridX, gridY));
                }
            }

            const float playerCenterX = playerObject.getX() + PLAYER_HALF_SIZE;
            const float playerCenterY = playerObject.getY() + PLAYER_HALF_SIZE;

            for (auto& loot : loots) {
                if (loot.ramasse) {
                    continue;
                }

                const float dx = playerCenterX - loot.position.x;
                const float dy = playerCenterY - loot.position.y;
                const float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < TILE_SIZE / 2.f && playerObject.addItem(loot.item)) {
                    loot.ramasse = true;
                }
            }

            for (auto& mechant : mechants) {
                const bool etaitActif = mechant.actif;

                mechant.verifierProximite(playerCenterX, playerCenterY, ENEMY_ACTIVATION_DISTANCE);
                mechant.update(deltaTime, playerCenterX, playerCenterY);
                if (mechant.toucheJoueur(playerCenterX, playerCenterY)) {
                    playerObject.mourir();
                    std::cout << "Le chevalier a ete touche par " << mechant.nom << "." << std::endl;
                    break;
                }
                mechant.desintegrerSiTropLoin(playerCenterX, playerCenterY, ENEMY_DESINTEGRATION_DISTANCE);

                if (etaitActif && !mechant.actif && mechant.etat == EtatMechant::VAINCU) {
                    loots.push_back(Loot{sf::Vector2f(mechant.x, mechant.y), "loot mechant", false});
                }
            }
        }

        window.clear();
        view.setCenter({playerObject.getX() + PLAYER_HALF_SIZE, playerObject.getY() + PLAYER_HALF_SIZE});
        window.setView(view);
        int centerX = static_cast<int>(std::floor((playerObject.getX() + PLAYER_HALF_SIZE) / TILE_SIZE));
        int centerY = static_cast<int>(std::floor((playerObject.getY() + PLAYER_HALF_SIZE) / TILE_SIZE));
        // dessin du monde
        world.draw(window, {playerObject.getX(), playerObject.getY()});

        for (int y = centerY - PICKUP_DRAW_RADIUS; y <= centerY + PICKUP_DRAW_RADIUS; y++) {
            for (int x = centerX - PICKUP_DRAW_RADIUS; x <= centerX + PICKUP_DRAW_RADIUS; x++) {
                if (!world.isWalkableTile(x, y)) {
                    continue;
                }

                const char collectibleTile = generateCollectibleTile(x, y);

                if (collectibleTile == 'p' && !containsPosition(piecesRamassees, x, y)) {
                    drawCoin(window, sf::Vector2f(x * TILE_SIZE + 25.f, y * TILE_SIZE + 25.f), 9.f);
                } else if (collectibleTile == 'n' && !containsPosition(pommesMangees, x, y)) {
                    drawFood(window, sf::Vector2f(x * TILE_SIZE + 25.f, y * TILE_SIZE + 25.f));
                } else if (collectibleTile == 'b' && !containsPosition(bandagesUtilises, x, y)) {
                    drawBandage(window, sf::Vector2f(x * TILE_SIZE + 25.f, y * TILE_SIZE + 25.f));
                }
            }
        }

        for (const auto& loot : loots) {
            if (!loot.ramasse) {
                drawLootSign(window, loot.position);
            }
        }

        for (const auto& mechant : mechants) {
            mechant.draw(window);
        }

        // si le png charge, on dessine le chevalier
        if (playerTextureLoaded) {
            sf::Sprite player(playerTexture);
            player.setTextureRect(sf::IntRect(
                sf::Vector2i(0, 0),
                sf::Vector2i(PLAYER_FRAME_WIDTH, PLAYER_FRAME_HEIGHT)
            ));
            player.setScale(sf::Vector2f(PLAYER_SCALE, PLAYER_SCALE));
            player.setPosition({
                playerObject.getX() + PLAYER_DRAW_X_OFFSET,
                playerObject.getY()
            });
            window.draw(player);
        } else {
            // sinon on garde le carre rouge pour depanner
            sf::RectangleShape player({PLAYER_SIZE, PLAYER_SIZE});
            player.setPosition({playerObject.getX(), playerObject.getY()});
            player.setFillColor(sf::Color::Red);
            window.draw(player);
        }
        window.setView(uiView);

        sf::RectangleShape backBar(sf::Vector2f(200.f, 20.f));
        backBar.setFillColor(sf::Color(50, 50, 50));
        backBar.setPosition(sf::Vector2f(10.f, 10.f));
        window.draw(backBar);

        const float lifeWidth = (static_cast<float>(playerObject.vie) / 100.f) * 200.f;
        sf::RectangleShape lifeBar(sf::Vector2f(lifeWidth, 20.f));
        lifeBar.setFillColor(sf::Color::Red);
        lifeBar.setPosition(sf::Vector2f(10.f, 10.f));
        window.draw(lifeBar);

        sf::RectangleShape backFaim(sf::Vector2f(200.f, 10.f));
        backFaim.setFillColor(sf::Color(50, 50, 50));
        backFaim.setPosition(sf::Vector2f(10.f, 35.f));
        window.draw(backFaim);

        const float faimAffichee = std::min(playerObject.faim, 100.0f);
        sf::RectangleShape faimBar(sf::Vector2f((faimAffichee / 100.f) * 200.f, 10.f));
        faimBar.setFillColor(sf::Color(255, 165, 0));
        faimBar.setPosition(sf::Vector2f(10.f, 35.f));
        window.draw(faimBar);

        if (fontLoaded) {
            sf::Text levelText(font, "Niveau " + std::to_string(playerObject.niveau), 18);
            levelText.setFillColor(sf::Color::White);
            levelText.setPosition(sf::Vector2f(10.f, 48.f));
            window.draw(levelText);

            sf::Text coinText(font, " : " + std::to_string(playerObject.argent), 20);
            coinText.setFillColor(sf::Color::White);
            coinText.setPosition(sf::Vector2f(30.f, 70.f));
            window.draw(coinText);
        }

        drawCoin(window, sf::Vector2f(18.f, 83.f), 8.f);

        for (int i = 0; i < 4; i++) {
            sf::RectangleShape slot({45.f, 45.f});
            slot.setPosition({10.f + i * 60.f, 540.f});
            slot.setFillColor(sf::Color(50, 50, 50));
            window.draw(slot);
        }

        if (playerObject.vie <= 0) {
            sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(overlay);

            if (fontLoaded) {
                window.draw(deathText);
            }
        }

        window.display();
    }
}

