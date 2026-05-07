#pragma once
#include "Biome.hpp"

class EastBiome : public Biome {
public:
    EastBiome();
    virtual char generateTile(int tx, int ty) const override;
};
