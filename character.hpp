#ifndef CHARACTERS_HPP
#define CHARACTERS_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include "entity.hpp"

// ============================================================
//  ÉTAT D'UN MÉCHANT
// ============================================================
enum class EtatMechant {
    ATTENTE,   // Immobile sur la map
    COMBAT,    // Poursuit le joueur
    VAINCU     // Éliminé
};

// ============================================================
//  CLASSE MÉCHANT
// ============================================================
class Mechant {
public:
    std::string nom;
    int         niveau;        // 1 à 5
    float       x, y;
    float       vitesse;
    EtatMechant etat;
    bool        actif;         // false = déjà vaincu

    // Récompense XP selon le niveau
    int xpRecompense() const { return niveau * 5; }

    Mechant(const std::string& nom, int niveau, float x, float y)
        : nom(nom), niveau(niveau), x(x), y(y),
          vitesse(50.f * niveau), // Plus fort = plus rapide
          etat(EtatMechant::ATTENTE), actif(true)
    {}

    // Mise à jour : poursuite du joueur pendant un combat
    void update(float deltaTime, float joueurX, float joueurY) {
        if (!actif || etat != EtatMechant::COMBAT) return;

        float dx = joueurX - x;
        float dy = joueurY - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > 1.f) {
            x += (dx / dist) * vitesse * deltaTime;
            y += (dy / dist) * vitesse * deltaTime;
        }
    }

    // Déclenche le combat si le joueur est proche
    void verifierProximite(float joueurX, float joueurY, float distanceActivation = 60.f) {
        if (!actif || etat != EtatMechant::ATTENTE) return;

        float dx = joueurX - x;
        float dy = joueurY - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < distanceActivation) {
            etat = EtatMechant::COMBAT;
            std::cout << "⚔️  Combat initié avec " << nom
                      << " (niveau " << niveau << ") !" << std::endl;
        }
    }

    // Vérifie si le méchant touche le joueur
    void desintegrerSiTropLoin(float joueurX, float joueurY, float distanceMax) {
        if (!actif || etat != EtatMechant::COMBAT) return;

        float dx = joueurX - x;
        float dy = joueurY - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > distanceMax) {
            actif = false;
            etat = EtatMechant::VAINCU;
            std::cout << nom << " se desintegre." << std::endl;
        }
    }

    bool toucheJoueur(float joueurX, float joueurY) const {
        if (!actif || etat != EtatMechant::COMBAT) return false;
        float dx = joueurX - x;
        float dy = joueurY - y;
        return std::sqrt(dx * dx + dy * dy) < 40.f;
    }

    // Dessin : croix rouge
    void draw(sf::RenderWindow& window) const {
        if (!actif) return;

        sf::Color couleur = (etat == EtatMechant::COMBAT)
                            ? sf::Color(255, 50, 50)
                            : sf::Color(180, 0, 0);

        // Barre horizontale
        sf::RectangleShape h(sf::Vector2f(40.f, 12.f));
        h.setFillColor(couleur);
        h.setPosition(sf::Vector2f(x - 20.f, y - 6.f));
        window.draw(h);

        // Barre verticale
        sf::RectangleShape v(sf::Vector2f(12.f, 40.f));
        v.setFillColor(couleur);
        v.setPosition(sf::Vector2f(x - 6.f, y - 20.f));
        window.draw(v);

        // Indicateur de niveau : petits carrés au-dessus
        for (int i = 0; i < niveau; i++) {
            sf::RectangleShape dot(sf::Vector2f(6.f, 6.f));
            dot.setFillColor(sf::Color::Yellow);
            dot.setPosition(sf::Vector2f(x - (niveau * 8.f) / 2.f + i * 8.f, y - 30.f));
            window.draw(dot);
        }
    }
};

// ============================================================
//  TYPE D'ALLIÉ
// ============================================================
enum class TypeAllie {
    GUERISSEUR,  // Donne une potion (récupère XP perdu)
    DEALEUR      // Donne de l'XP directement
};

// ============================================================
//  CLASSE ALLIÉ
// ============================================================
class Allie {
public:
    std::string nom;
    TypeAllie   type;
    float       x, y;
    int         don;          // Quantité donnée
    bool        aDejaParle;   // N'offre son item qu'une seule fois

    Allie(const std::string& nom, TypeAllie type, float x, float y, int don)
        : nom(nom), type(type), x(x), y(y), don(don), aDejaParle(false)
    {}

    // Interaction si le joueur est proche
    void verifierProximite(float joueurX, float joueurY, Player& joueur) {
        if (aDejaParle) return;

        float dx = joueurX - x;
        float dy = joueurY - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < 60.f) {
            aDejaParle = true;
            if (type == TypeAllie::GUERISSEUR) {
                std::cout << "💚 " << nom << " : \"Bonjour ! Tiens, une Potion de Vigueur +"
                          << don << " XP !\"" << std::endl;
                joueur.ajouterArgent(don);
            }
            else {
                std::cout << "💛 " << nom << " : \"Bonjour ! Tiens, "
                          << don << " pièces d'or !\"" << std::endl;
                joueur.ajouterArgent(don);
            }
        }
    }

    // Dessin : croix verte
    void draw(sf::RenderWindow& window) const {
        if (aDejaParle) return;

        sf::Color couleur = (type == TypeAllie::GUERISSEUR)
                            ? sf::Color(0, 200, 100)
                            : sf::Color(100, 220, 0);

        sf::RectangleShape h(sf::Vector2f(40.f, 12.f));
        h.setFillColor(couleur);
        h.setPosition(sf::Vector2f(x - 20.f, y - 6.f));
        window.draw(h);

        sf::RectangleShape v(sf::Vector2f(12.f, 40.f));
        v.setFillColor(couleur);
        v.setPosition(sf::Vector2f(x - 6.f, y - 20.f));
        window.draw(v);
    }
};

#endif
