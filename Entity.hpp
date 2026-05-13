#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Pour std::min

// --- CLASSE DE BASE : ENTITY ---
class Entity {
protected:
    float x, y;

public:
    Entity(float x, float y) : x(x), y(y) {}

    // Un destructeur virtuel est essentiel pour éviter les fuites mémoire en héritage
    virtual ~Entity() {}

    // On garde la version avec deltaTime car c'est plus précis pour le mouvement/survie
    virtual void update(float deltaTime) = 0;

    float getX() const { return x; }
    float getY() const { return y; }

    void setPos(float nx, float ny) {
        x = nx;
        y = ny;
    }

    // Version "move" pour les déplacements relatifs
    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }
};

// --- CLASSE DERIVEE : PLAYER ---
class Player : public Entity {
private:
    float vieInterne;       // Pour la précision des dégâts par seconde
    std::string inventory[4]; // Ton système d'inventaire de la v1

public:
    int vie = 100;
    float faim = 0.0f;      // Jauge de faim
    int argent = 0;         // Ton trésor
    int niveau = 1;
    float vitesseBase = 200.0f;

    Player(float x, float y) : Entity(x, y), vieInterne(100.0f) {
        // Initialisation de l'inventaire
        for (int i = 0; i < 4; i++) {
            inventory[i] = "empty";
        }
    }

    // Gestion de l'inventaire
    bool addItem(const std::string& item) {
        for (int i = 0; i < 4; i++) {
            if (inventory[i] == "empty") {
                inventory[i] = item;
                std::cout << "Objet ajoute : " << item << std::endl;
                return true;
            }
        }
        std::cout << "Inventaire plein !" << std::endl;
        return false;
    }

    // Mise à jour de la survie
    void updateStats(float deltaTime) {
        // 1. La faim monte (2 points par seconde)
        if (faim < 110.0f) {
            faim += 2.0f * deltaTime;
        }

        // 2. Si faim > 100, perte de vie (5 PV par seconde)
        if (faim >= 100.0f) {
            vieInterne -= 5.0f * deltaTime;
            vie = static_cast<int>(vieInterne);
        }

        // Sécurité
        if (vie < 0) {
            vie = 0;
            vieInterne = 0.0f;
        }
    }

    void ajouterArgent(int montant) {
        argent += montant;
        niveau = 1 + argent / 15;
    }

    float getMultiplicateurVitesse() const {
        return 1.0f + (niveau - 1) * 0.2f;
    }

    void manger() {
        faim = 0.0f;
    }

    void soigner() {
        vieInterne = 100.0f;
        vie = 100;
    }

    // Implémentation obligatoire de update
    void mourir() {
        vieInterne = 0.0f;
        vie = 0;
    }

    void update(float deltaTime) override {
        updateStats(deltaTime);
    }
};

#endif
