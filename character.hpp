#ifndef CHARACTERS_HPP
#define CHARACTERS_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <string>

// ============================================================
//  ETAT D'UN MECHANT
// ============================================================
/**
 * Etats possibles d'un ennemi pendant la partie.
 */
enum class EtatMechant {
    ATTENTE,   // Immobile sur la map
    COMBAT,    // Poursuit le joueur
    VAINCU     // Elimine
};

// ============================================================
//  CLASSE MECHANT
// ============================================================
/**
 * Ennemi place sur la carte, capable de detecter et poursuivre le joueur.
 */
class Mechant {
public:
    std::string nom;
    int         niveau;        // 1 a 5
    float       x, y;
    float       vitesse;
    sf::Color   couleur;
    EtatMechant etat;
    bool        actif;         // false = deja vaincu

    /**
     * Calcule la recompense theorique en XP selon le niveau de l'ennemi.
     */
    int xpRecompense() const { return niveau * 5; }

    /**
     * Cree un ennemi avec son nom, son niveau et sa position de depart.
     */
    Mechant(const std::string& nom, int niveau, float x, float y, sf::Color couleur)
        : nom(nom), niveau(niveau), x(x), y(y),
          vitesse(50.f * niveau), // Plus fort = plus rapide
          couleur(couleur),
          etat(EtatMechant::ATTENTE), actif(true)
    {}

    /**
     * Fait avancer l'ennemi vers le joueur quand il est en combat.
     */
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

    /**
     * Declenche le combat si le joueur entre dans la distance d'activation.
     */
    void verifierProximite(float joueurX, float joueurY, float distanceActivation = 80.f) {
        if (!actif || etat != EtatMechant::ATTENTE) return;

        float dx = joueurX - x;
        float dy = joueurY - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < distanceActivation) {
            etat = EtatMechant::COMBAT;
            std::cout << "Combat initie avec " << nom
                      << " (niveau " << niveau << ") !" << std::endl;
        }
    }

    /**
     * Desactive l'ennemi si le joueur s'eloigne trop pendant le combat.
     */
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

    /**
     * Dit si l'ennemi touche le joueur pendant le combat.
     */
    bool toucheJoueur(float joueurX, float joueurY) const {
        if (!actif || etat != EtatMechant::COMBAT) return false;

        float dx = joueurX - x;
        float dy = joueurY - y;
        return std::sqrt(dx * dx + dy * dy) < 40.f;
    }

    /**
     * Dessine l'ennemi, son indicateur de niveau et son nom.
     */
    void draw(sf::RenderWindow& window, const sf::Font* font = nullptr) const {
        if (!actif) return;

        sf::RectangleShape h(sf::Vector2f(40.f, 12.f));
        h.setFillColor(couleur);
        h.setPosition(sf::Vector2f(x - 20.f, y - 6.f));
        window.draw(h);

        sf::RectangleShape v(sf::Vector2f(12.f, 40.f));
        v.setFillColor(couleur);
        v.setPosition(sf::Vector2f(x - 6.f, y - 20.f));
        window.draw(v);

        for (int i = 0; i < niveau; i++) {
            sf::RectangleShape dot(sf::Vector2f(6.f, 6.f));
            dot.setFillColor(sf::Color::Yellow);
            dot.setPosition(sf::Vector2f(x - (niveau * 8.f) / 2.f + i * 8.f, y - 30.f));
            window.draw(dot);
        }

        if (font != nullptr) {
            sf::Text nameText(*font, nom, 12);
            nameText.setFillColor(sf::Color::White);
            nameText.setOutlineColor(sf::Color::Black);
            nameText.setOutlineThickness(1.f);

            sf::FloatRect bounds = nameText.getLocalBounds();
            nameText.setOrigin({
                bounds.position.x + bounds.size.x / 2.f,
                bounds.position.y + bounds.size.y / 2.f
            });
            nameText.setPosition({x, y - 48.f});
            window.draw(nameText);
        }
    }
};

#endif
