#pragma once
#include "Biome.hpp"

class SouthBiome : public Biome {
public:
    SouthBiome();
    virtual char getGroundTile() const override;
    virtual char generateOverlay(int tx, int ty) const override;
};
