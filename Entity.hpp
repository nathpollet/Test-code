#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Pour std::min

// --- CLASSE DE BASE : ENTITY ---
/**
 * Classe de base pour tous les objets qui ont une position dans le monde.
 */
class Entity {
protected:
    float x, y;
    std::string nom;

public:
    /**
     * Cree une entite a une position donnee avec un nom affichable.
     */
    Entity(float x, float y, const std::string& nom = "Entite") : x(x), y(y), nom(nom) {}

    // Un destructeur virtuel est essentiel pour éviter les fuites mémoire en héritage
    /**
     * Permet de detruire correctement les classes derivees via un pointeur Entity.
     */
    virtual ~Entity() {}

    // On garde la version avec deltaTime car c'est plus précis pour le mouvement/survie
    /**
     * Met a jour l'entite en fonction du temps ecoule depuis la derniere frame.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * Renvoie la position horizontale de l'entite.
     */
    float getX() const { return x; }
    /**
     * Renvoie la position verticale de l'entite.
     */
    float getY() const { return y; }

    /**
     * Renvoie le nom affiche au-dessus de l'entite.
     */
    const std::string& getNom() const { return nom; }

    /**
     * Place directement l'entite a une nouvelle position.
     */
    void setPos(float nx, float ny) {
        x = nx;
        y = ny;
    }

    // Version "move" pour les déplacements relatifs
    /**
     * Decale l'entite par rapport a sa position actuelle.
     */
    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }
};

// --- CLASSE DERIVEE : PLAYER ---
/**
 * Joueur controle par l'utilisateur, avec vie, faim, argent et inventaire.
 */
class Player : public Entity {
private:
    float vieInterne;       // Pour la précision des dégâts par seconde
    std::string inventory[4]; // Ton système d'inventaire de la v1

public:
    int vie = 100;
    float faim = 0.0f;      // Jauge de faim
    int argent = 0;         // Ton trésor
    int niveau = 1;
    float vitesseBase = 500.0f;

    /**
     * Cree le joueur a une position donnee et initialise son inventaire.
     */
    Player(float x, float y, const std::string& nom = "Duncan the Tall") : Entity(x, y, nom), vieInterne(100.0f) {
        // Initialisation de l'inventaire
        for (int i = 0; i < 4; i++) {
            inventory[i] = "empty";
        }
    }

    // Gestion de l'inventaire
    /**
     * Ajoute un objet dans le premier emplacement libre de l'inventaire.
     */
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
    /**
     * Fait progresser la faim et applique les degats si le joueur a trop faim.
     */
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

    /**
     * Ajoute de l'argent et recalcule le niveau du joueur.
     */
    void ajouterArgent(int montant) {
        argent += montant;
        niveau = std::max(niveau, 1 + argent / 15);
    }

    /**
     * Ajoute directement un niveau au joueur.
     */
    void gagnerNiveau() {
        niveau += 1;
    }

    /**
     * Renvoie le bonus de vitesse donne par le niveau actuel.
     */
    float getMultiplicateurVitesse() const {
        return 1.0f + (niveau - 1) * 0.4f;
    }

    /**
     * Remet la jauge de faim a zero.
     */
    void manger() {
        faim = 0.0f;
    }

    /**
     * Remet la vie du joueur au maximum.
     */
    void soigner() {
        vieInterne = 100.0f;
        vie = 100;
    }

    /**
     * Vide tous les emplacements de l'inventaire.
     */
    void viderInventaire() {
        for (int i = 0; i < 4; i++) {
            inventory[i] = "empty";
        }
    }

    // Implémentation obligatoire de update
    /**
     * Force la mort du joueur.
     */
    void mourir() {
        vieInterne = 0.0f;
        vie = 0;
    }

    /**
     * Met a jour les statistiques de survie du joueur.
     */
    void update(float deltaTime) override {
        updateStats(deltaTime);
    }
};

#endif
