#include <SFML/Graphics.hpp>
#include "entity.hpp"
#include <cmath>
#include "world.hpp"



int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    sf::View view(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));
    Player playerObject(50.0f, 50.0f);
    World world(50.f, 11);
    sf::Clock clock;
    float speed = 100.0f;
    const float TILE_SIZE = 50.f;
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

float left = nextX;
float right = nextX + 49.f ;
float top = nextY;
float bottom = nextY + 49.f ;
int leftTile   = static_cast<int>(std::floor(left / TILE_SIZE));
int rightTile  = static_cast<int>(std::floor(right / TILE_SIZE));
int topTile    = static_cast<int>(std::floor(top / TILE_SIZE));
int bottomTile = static_cast<int>(std::floor(bottom / TILE_SIZE));
if (world.getTileAt(leftTile, topTile) != '#' &&
    world.getTileAt(rightTile, topTile) != '#' &&
    world.getTileAt(leftTile, bottomTile) != '#' &&
    world.getTileAt(rightTile, bottomTile) != '#') {
    playerObject.move(nextX - playerObject.getX(),
                      nextY - playerObject.getY());
}
        sf::RectangleShape player({50.f, 50.f});
        player.setPosition({nextX, nextY});
        player.setFillColor(sf::Color::Red);

        view.setCenter({nextX + 25.f, nextY + 25.f});
        window.setView(view);
        playerObject.addItem("wood");
        playerObject.addItem("stone");
        int renderDistance = 30; // ajuste (30, 50, etc.)
        int centerX = static_cast<int>(std::floor((playerObject.getX() + 25.f) / TILE_SIZE));
        int centerY = static_cast<int>(std::floor((playerObject.getY() + 25.f) / TILE_SIZE));
    // --- DESSIN DU MONDE ---
world.draw(window, {playerObject.getX(), playerObject.getY()});

        window.draw(player);
        window.setView(uiView);
        for (int i = 0; i < 4; i++) {
            sf::RectangleShape slot({50.f, 50.f});
            slot.setPosition({10.f + i * 60.f, 540.f});
            slot.setFillColor(sf::Color(50, 50, 50));
            window.draw(slot);
        }
        window.display();
    }
}

