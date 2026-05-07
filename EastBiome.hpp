#pragma once
#include "Biome.hpp"

class EastBiome : public Biome {
public:
    EastBiome();
    virtual char getGroundTile() const override;
    virtual char generateOverlay(int tx, int ty) const override;
};
