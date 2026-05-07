#pragma once
#include "Biome.hpp"

class NorthBiome : public Biome {
public:
    NorthBiome();
    virtual char getGroundTile() const override;
    virtual char generateOverlay(int tx, int ty) const override;
};
