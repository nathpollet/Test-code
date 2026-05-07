#pragma once
#include "Biome.hpp"

class WestBiome : public Biome {
public:
    WestBiome();
    virtual char getGroundTile() const override;
    virtual char generateOverlay(int tx, int ty) const override;
};
