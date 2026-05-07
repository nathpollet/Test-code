#include <SFML/Graphics.hpp>
#include "entity.hpp"
#include <cmath>
#include <iostream>
#include "world.hpp"



int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    sf::View view(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));
    Player playerObject(50.0f, 50.0f);
    World world(50.f, 11);
    sf::Clock clock;
    float speed = 100.0f;
    const float TILE_SIZE = 50.f;

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
    sf::View uiView(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        while (auto event = window.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
        window.close();
    }
}
    window.clear();
    float nextY = playerObject.getY();
    float nextX = playerObject.getX();
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
    nextY  -= speed * deltaTime;
}
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
    nextY += speed * deltaTime;
}
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
    nextX -= speed * deltaTime;
}
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
    nextX += speed * deltaTime;
}

// on bouge seulement si la hitbox passe
if (world.canMoveTo(nextX, nextY, PLAYER_SIZE, PLAYER_SIZE)) {
    playerObject.move(nextX - playerObject.getX(),
                      nextY - playerObject.getY());
}
        view.setCenter({playerObject.getX() + PLAYER_HALF_SIZE, playerObject.getY() + PLAYER_HALF_SIZE});
        window.setView(view);
        int centerX = static_cast<int>(std::floor((playerObject.getX() + PLAYER_HALF_SIZE) / TILE_SIZE));
        int centerY = static_cast<int>(std::floor((playerObject.getY() + PLAYER_HALF_SIZE) / TILE_SIZE));
    // dessin du monde
world.draw(window, {playerObject.getX(), playerObject.getY()});

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
        for (int i = 0; i < 4; i++) {
            sf::RectangleShape slot({45.f, 45.f});
            slot.setPosition({10.f + i * 60.f, 540.f});
            slot.setFillColor(sf::Color(50, 50, 50));
            window.draw(slot);
        }
        window.display();
    }
}

