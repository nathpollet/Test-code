#pragma once
#include "Biome.hpp"

class WestBiome : public Biome {
public:
    WestBiome();
    virtual char generateTile(int tx, int ty) const override;
};
