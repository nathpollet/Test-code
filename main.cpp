#include <SFML/Graphics.hpp>
#include "entity.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "world.hpp"
#include "character.hpp"
#include <fstream>

namespace {
/**
 * Objet laisse dans le monde, avec sa position et son etat de ramassage.
 */
struct Loot {
    sf::Vector2f position;
    std::string item;
    sf::Color couleur;
    bool ramasse;
};

/**
 * Couleur de Ramsay Bolton, utilisee aussi pour son loot.
 */
const sf::Color RAMSAY_COLOR(150, 35, 50);

/**
 * Couleur de Cersei Lannister, utilisee aussi pour son loot.
 */
const sf::Color CERSEI_COLOR(220, 175, 45);

/**
 * Couleur de Lysa Aryn, utilisee aussi pour son loot.
 */
const sf::Color LYSA_COLOR(70, 135, 220);

/**
 * Couleur de The Mountain, utilisee aussi pour son loot.
 */
const sf::Color MOUNTAIN_COLOR(125, 125, 135);

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

/**
 * Verifie si une position de grille est deja presente dans une liste.
 */
bool containsPosition(const std::vector<sf::Vector2i>& positions, int x, int y) {
    for (const auto& pos : positions) {
        if (pos.x == x && pos.y == y) {
            return true;
        }
    }

    return false;
}

/**
 * Dessine une piece ronde centree sur la position donnee.
 */
void drawCoin(sf::RenderWindow& window, sf::Vector2f center, float radius) {
    sf::CircleShape coin(radius);
    coin.setFillColor(sf::Color(235, 190, 35));
    coin.setOutlineThickness(2.f);
    coin.setOutlineColor(sf::Color(130, 90, 20));
    coin.setPosition({center.x - radius, center.y - radius});
    window.draw(coin);
}

/**
 * Dessine une petite pomme ramassable.
 */
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

/**
 * Dessine un bandage qui sert a soigner le joueur.
 */
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

/**
 * Dessine le marqueur visuel d'un loot laisse par un ennemi.
 */
void drawLootSign(sf::RenderWindow& window, sf::Vector2f center, sf::Color color) {
    sf::RectangleShape sign({22.f, 22.f});
    sign.setFillColor(color);
    sign.setOutlineThickness(2.f);
    sign.setOutlineColor(sf::Color(color.r / 2, color.g / 2, color.b / 2));
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

/**
 * Replace les quatre ennemis autour des portes avec leurs couleurs.
 */
void creerMechantsDeDepart(std::vector<Mechant>& mechants, float tileSize, int chunkSize, float enemyDoorDistance) {
    const int doorTile = chunkSize / 2;

    mechants.clear();
    mechants.push_back(Mechant("Ramsay Bolton", 1, (doorTile + 0.5f) * tileSize, (-1.f - enemyDoorDistance + 0.5f) * tileSize, RAMSAY_COLOR));
    mechants.push_back(Mechant("Cersei Lannister", 2, (doorTile + 0.5f) * tileSize, (chunkSize + enemyDoorDistance + 0.5f) * tileSize, CERSEI_COLOR));
    mechants.push_back(Mechant("Lysa Aryn", 3, (chunkSize + enemyDoorDistance + 0.5f) * tileSize, (doorTile + 0.5f) * tileSize, LYSA_COLOR));
    mechants.push_back(Mechant("The Mountain", 4, (-1.f - enemyDoorDistance + 0.5f) * tileSize, (doorTile + 0.5f) * tileSize, MOUNTAIN_COLOR));
}

void sauverPartie(const Player& player,
    const std::vector<sf::Vector2i>& pieces,
    const std::vector<sf::Vector2i>& pommes,
    const std::vector<sf::Vector2i>& bandages,
    const std::vector<Loot>& loots,
    const std::vector<sf::Color>& lootInventaire,
    const std::vector<Mechant>& mechants) {
    std::ofstream fichier("sauvegarde.dat");
    if (fichier.is_open()) {
        // 1. Joueur : X, Y, Vie, Faim, Argent
        fichier << player.getX() << " " << player.getY() << " "
            << player.vie << " " << player.faim << " " << player.argent << "\n";

        // 2. Pièces
        fichier << pieces.size() << "\n";
        for (const auto& pos : pieces) fichier << pos.x << " " << pos.y << " ";
        fichier << "\n";

        // 3. Pommes
        fichier << pommes.size() << "\n";
        for (const auto& pos : pommes) fichier << pos.x << " " << pos.y << " ";
        fichier << "\n";

        // 4. Bandages
        fichier << bandages.size() << "\n";
        for (const auto& pos : bandages) fichier << pos.x << " " << pos.y << " ";
        fichier << "\n";

        // 5. Etat des ennemis
        fichier << mechants.size() << "\n";
        for (const auto& mechant : mechants) {
            fichier << mechant.actif << " "
                    << static_cast<int>(mechant.etat) << " "
                    << mechant.x << " "
                    << mechant.y << "\n";
        }

        // 6. Loots encore presents au sol
        fichier << loots.size() << "\n";
        for (const auto& loot : loots) {
            fichier << loot.position.x << " "
                    << loot.position.y << " "
                    << static_cast<int>(loot.couleur.r) << " "
                    << static_cast<int>(loot.couleur.g) << " "
                    << static_cast<int>(loot.couleur.b) << " "
                    << loot.ramasse << "\n";
        }

        // 7. Loots deja ranges dans l'inventaire visuel
        fichier << lootInventaire.size() << "\n";
        for (const auto& couleur : lootInventaire) {
            fichier << static_cast<int>(couleur.r) << " "
                    << static_cast<int>(couleur.g) << " "
                    << static_cast<int>(couleur.b) << "\n";
        }

        fichier.close();
        std::cout << "Monde et inventaire sauvegardes !" << std::endl;
    }
}

void chargerPartie(Player& player,
    std::vector<sf::Vector2i>& pieces,
    std::vector<sf::Vector2i>& pommes,
    std::vector<sf::Vector2i>& bandages,
    std::vector<Loot>& loots,
    std::vector<sf::Color>& lootInventaire,
    std::vector<Mechant>& mechants) {
    std::ifstream fichier("sauvegarde.dat");
    if (fichier.is_open()) {
        float px, py, f;
        int a;
        float v;

        if (fichier >> px >> py >> v >> f >> a) {
            player.setPos(px, py);
            player.vie = static_cast<int>(v);
            player.faim = f;
            player.argent = a;
            player.niveau = std::max(player.niveau, 1 + player.argent / 15);
        }

        auto lireListe = [&](std::vector<sf::Vector2i>& liste) {
            size_t taille;
            if (fichier >> taille) {
                liste.clear();
                for (size_t i = 0; i < taille; ++i) {
                    int x, y;
                    fichier >> x >> y;
                    liste.push_back(sf::Vector2i(x, y));
                }
            }
            };

        lireListe(pieces);
        lireListe(pommes);
        lireListe(bandages);

        size_t nombreMechants;
        if (fichier >> nombreMechants) {
            const size_t limite = std::min(nombreMechants, mechants.size());
            for (size_t i = 0; i < nombreMechants; i++) {
                bool actif;
                int etat;
                float x, y;
                fichier >> actif >> etat >> x >> y;

                if (i < limite) {
                    mechants[i].actif = actif;
                    mechants[i].etat = static_cast<EtatMechant>(etat);
                    mechants[i].x = x;
                    mechants[i].y = y;
                }
            }
        }

        size_t nombreLoots;
        if (fichier >> nombreLoots) {
            loots.clear();
            for (size_t i = 0; i < nombreLoots; i++) {
                float x, y;
                int r, g, b;
                bool ramasse;
                fichier >> x >> y >> r >> g >> b >> ramasse;
                loots.push_back(Loot{sf::Vector2f(x, y), "Loot ennemi", sf::Color(r, g, b), ramasse});
            }
        }

        size_t nombreLootsInventaire;
        if (fichier >> nombreLootsInventaire) {
            lootInventaire.clear();
            for (size_t i = 0; i < nombreLootsInventaire; i++) {
                int r, g, b;
                fichier >> r >> g >> b;
                lootInventaire.push_back(sf::Color(r, g, b));
                player.gagnerNiveau();
            }
        }

        fichier.close();
        std::cout << "Monde restaure avec succes." << std::endl;
    }
}

void reinitialiserJeu(Player& player,
    std::vector<sf::Vector2i>& pieces,
    std::vector<sf::Vector2i>& pommes,
    std::vector<sf::Vector2i>& bandages,
    std::vector<Loot>& loots,
    std::vector<sf::Color>& lootInventaire,
    std::vector<Mechant>& mechants,
    float tileSize,
    int chunkSize,
    float enemyDoorDistance) {
    // 1. Remettre les stats du joueur
    player.setPos(250.f, 250.f);
    player.soigner();
    player.faim = 0.0f;
    player.argent = 0;
    player.niveau = 1;
    player.viderInventaire();

    // 2. Vider les listes d'objets ramassés pour qu'ils réapparaissent
    pieces.clear();
    pommes.clear();
    bandages.clear();
    loots.clear();
    lootInventaire.clear();
    creerMechantsDeDepart(mechants, tileSize, chunkSize, enemyDoorDistance);

    // 3. Optionnel : Supprimer le fichier de sauvegarde pour repartir de zéro
    std::remove("sauvegarde.dat");
}
/**
 * Dessine un petit nom centre au-dessus d'une entite.
 */
void drawNameLabel(sf::RenderWindow& window, const sf::Font& font, const std::string& name, sf::Vector2f position) {
    sf::Text nameText(font, name, 12);
    nameText.setFillColor(sf::Color::White);
    nameText.setOutlineColor(sf::Color::Black);
    nameText.setOutlineThickness(1.f);

    sf::FloatRect bounds = nameText.getLocalBounds();
    nameText.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });
    nameText.setPosition(position);
    window.draw(nameText);
}

/**
 * Applique un deplacement axe par axe pour glisser contre les obstacles.
 */
sf::Vector2f resolveSlidingMovement(const World& world, sf::Vector2f position, sf::Vector2f delta, float width, float height) {
    sf::Vector2f resolved = position;

    if (delta.x != 0.f && world.canMoveTo(resolved.x + delta.x, resolved.y, width, height)) {
        resolved.x += delta.x;
    }

    if (delta.y != 0.f && world.canMoveTo(resolved.x, resolved.y + delta.y, width, height)) {
        resolved.y += delta.y;
    }

    return resolved;
}
}

/**
 * Lance la fenetre, gere les controles du joueur et dessine la scene.
 */
int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
    sf::View view(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));
    Player playerObject(50.0f, 50.0f, "Duncan the Tall");
    World world(50.f, 11);
    sf::Clock clock;
    float speed = 100.0f;
    const float TILE_SIZE = 50.f;
    const int PICKUP_DRAW_RADIUS = 12;
    const float ENEMY_ACTIVATION_DISTANCE = 4.f * TILE_SIZE;
    const float ENEMY_DESINTEGRATION_DISTANCE = 5.f * TILE_SIZE;
    const float ENEMY_DOOR_DISTANCE = 25.f;
    const int CHUNK_SIZE = 11;
    const int ENEMY_LOOT_COUNT = 4;

    std::vector<sf::Vector2i> piecesRamassees;
    std::vector<sf::Vector2i> pommesMangees;
    std::vector<sf::Vector2i> bandagesUtilises;
    std::vector<Loot> loots;
    std::vector<sf::Color> lootInventaire;
    std::vector<Mechant> mechants;
    creerMechantsDeDepart(mechants, TILE_SIZE, CHUNK_SIZE, ENEMY_DOOR_DISTANCE);

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


    chargerPartie(playerObject, piecesRamassees, pommesMangees, bandagesUtilises, loots, lootInventaire, mechants);
    bool victoireDejaAffichee = static_cast<int>(lootInventaire.size()) >= ENEMY_LOOT_COUNT;
    bool afficherVictoire = false;
    sf::Clock victoireClock;

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

    sf::RectangleShape boutonRejouer({ 200.f, 50.f });
    boutonRejouer.setFillColor(sf::Color::Red);
    boutonRejouer.setOrigin({ 100.f, 25.f });
    boutonRejouer.setPosition({ 400.f, 350.f });

    sf::Text boutonRejouerText(font, "rejouer", 25);
    boutonRejouerText.setFillColor(sf::Color::White);
    sf::FloatRect boutonTextRect = boutonRejouerText.getLocalBounds();
    boutonRejouerText.setOrigin({
        boutonTextRect.position.x + boutonTextRect.size.x / 2.f,
        boutonTextRect.position.y + boutonTextRect.size.y / 2.f
    });
    boutonRejouerText.setPosition(boutonRejouer.getPosition());

    sf::Text victoireText(font, "Victoire vous avez vaincu tous vos ennemis!", 24);
    victoireText.setFillColor(sf::Color::Yellow);
    victoireText.setOutlineColor(sf::Color::Black);
    victoireText.setOutlineThickness(2.f);
    sf::FloatRect victoireBounds = victoireText.getLocalBounds();
    victoireText.setOrigin({
        victoireBounds.position.x + victoireBounds.size.x / 2.f,
        victoireBounds.position.y + victoireBounds.size.y / 2.f
    });
    victoireText.setPosition({400.f, 120.f});

    sf::View uiView(sf::FloatRect({0.f, 0.f}, {800.f, 600.f}));
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                sauverPartie(playerObject, piecesRamassees, pommesMangees, bandagesUtilises, loots, lootInventaire, mechants);
                window.close();
            }
            if (playerObject.vie <= 0) {
                if (auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        // On récupère la position de la souris par rapport à la vue UI
                        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView);

                        if (boutonRejouer.getGlobalBounds().contains(mousePos)) {
                            reinitialiserJeu(
                                playerObject,
                                piecesRamassees,
                                pommesMangees,
                                bandagesUtilises,
                                loots,
                                lootInventaire,
                                mechants,
                                TILE_SIZE,
                                CHUNK_SIZE,
                                ENEMY_DOOR_DISTANCE
                            );
                            victoireDejaAffichee = false;
                            afficherVictoire = false;
                        }
                    }
                }
            }
        }
        if (playerObject.vie > 0) {
            playerObject.updateStats(deltaTime);

            const float currentSpeed = speed * playerObject.getMultiplicateurVitesse();
            sf::Vector2f playerDelta(0.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
                playerDelta.y -= currentSpeed * deltaTime;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                playerDelta.y += currentSpeed * deltaTime;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
                playerDelta.x -= currentSpeed * deltaTime;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                playerDelta.x += currentSpeed * deltaTime;
            }

            const sf::Vector2f playerPosition(playerObject.getX(), playerObject.getY());
            const sf::Vector2f resolvedPlayerPosition = resolveSlidingMovement(
                world,
                playerPosition,
                playerDelta,
                PLAYER_SIZE,
                PLAYER_SIZE
            );
            playerObject.setPos(resolvedPlayerPosition.x, resolvedPlayerPosition.y);

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
                    const int ancienNombreLoots = static_cast<int>(lootInventaire.size());
                    loot.ramasse = true;
                    playerObject.gagnerNiveau();
                    lootInventaire.push_back(loot.couleur);
                    std::cout << loot.item << " ramasse ! Niveau : " << playerObject.niveau << std::endl;

                    if (!victoireDejaAffichee &&
                        ancienNombreLoots < ENEMY_LOOT_COUNT &&
                        static_cast<int>(lootInventaire.size()) >= ENEMY_LOOT_COUNT) {
                        victoireDejaAffichee = true;
                        afficherVictoire = true;
                        victoireClock.restart();
                    }
                }
            }

            for (auto& mechant : mechants) {
                const bool etaitActif = mechant.actif;

                mechant.verifierProximite(playerCenterX, playerCenterY, ENEMY_ACTIVATION_DISTANCE);
                const float oldEnemyX = mechant.x;
                const float oldEnemyY = mechant.y;
                mechant.update(deltaTime, playerCenterX, playerCenterY);
                const sf::Vector2f oldEnemyTopLeft(oldEnemyX - PLAYER_HALF_SIZE, oldEnemyY - PLAYER_HALF_SIZE);
                const sf::Vector2f enemyDelta(mechant.x - oldEnemyX, mechant.y - oldEnemyY);
                const sf::Vector2f resolvedEnemyTopLeft = resolveSlidingMovement(
                    world,
                    oldEnemyTopLeft,
                    enemyDelta,
                    PLAYER_SIZE,
                    PLAYER_SIZE
                );
                mechant.x = resolvedEnemyTopLeft.x + PLAYER_HALF_SIZE;
                mechant.y = resolvedEnemyTopLeft.y + PLAYER_HALF_SIZE;

                if (mechant.toucheJoueur(playerCenterX, playerCenterY)) {
                    playerObject.mourir();
                    std::cout << "Le chevalier a ete touche par " << mechant.nom << "." << std::endl;
                    break;
                }
                mechant.desintegrerSiTropLoin(playerCenterX, playerCenterY, ENEMY_DESINTEGRATION_DISTANCE);

                if (etaitActif && !mechant.actif && mechant.etat == EtatMechant::VAINCU) {
                    loots.push_back(Loot{sf::Vector2f(mechant.x, mechant.y), "Loot de " + mechant.nom, mechant.couleur, false});
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
                drawLootSign(window, loot.position, loot.couleur);
            }
        }

        for (const auto& mechant : mechants) {
            mechant.draw(window, fontLoaded ? &font : nullptr);
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

        if (fontLoaded) {
            drawNameLabel(
                window,
                font,
                playerObject.getNom(),
                {playerObject.getX() + PLAYER_HALF_SIZE, playerObject.getY() - 14.f}
            );
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

            if (i < static_cast<int>(lootInventaire.size())) {
                drawLootSign(window, {32.5f + i * 60.f, 562.5f}, lootInventaire[i]);
            }
        }

        if (afficherVictoire && victoireClock.getElapsedTime().asSeconds() >= 3.f) {
            afficherVictoire = false;
        }

        if (fontLoaded && afficherVictoire) {
            window.draw(victoireText);
        }

        if (playerObject.vie <= 0) {
            sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(overlay);

            if (fontLoaded) {
                window.draw(deathText);
                window.draw(boutonRejouer);
                window.draw(boutonRejouerText);
            }
        }

        window.display();
    }
}
