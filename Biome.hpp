#pragma once

class Biome {
public:
    virtual ~Biome() = default;

    // le sol de base du biome
    virtual char getGroundTile() const = 0;

    // ce qu'on pose par-dessus, ou rien
    virtual char generateOverlay(int tx, int ty) const = 0;
};
