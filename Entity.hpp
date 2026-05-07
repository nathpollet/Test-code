#include <iostream>
#ifndef ENTITY_HPP
#define ENTITY_HPP

class Entity {
private:
    float x;
    float y;
public:
    Entity(float x, float y) : x(x), y(y) {}
    
    virtual void update() {}

    float getX() const { return x; }
    float getY() const { return y; }  
    void move(float dx, float dy) {
    x += dx;
    y += dy;
}
};


class Player : public Entity {
private:
    std::string inventory[4];

public:
    Player(float x, float y) : Entity(x, y) {
        for (int i = 0; i < 4; i++) {
            inventory[i] = "empty";
        }
    }

    void update() override {
        std::cout << "Player update\n";
    }
    bool addItem(const std::string& item) {
    for (int i = 0; i < 4; i++) {
        if (inventory[i] == "empty") {
            inventory[i] = item;
            return true;
        }
    }
    return false;

}
};
#endif