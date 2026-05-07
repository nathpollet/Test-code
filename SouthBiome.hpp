#pragma once
#include "Biome.hpp"

class SouthBiome : public Biome {
public:
    SouthBiome();
    virtual char generateTile(int tx, int ty) const override;
};
