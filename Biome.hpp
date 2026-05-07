#pragma once

class Biome {
public:
    virtual ~Biome() = default;

    // Génère le type de tuile pour un tile donné
    // À implémenter dans les classes filles
    virtual char generateTile(int tx, int ty) const = 0;
};
