#pragma once
#include "Biome.hpp"

class NorthBiome : public Biome {
public:
    NorthBiome();
    virtual char generateTile(int tx, int ty) const override;
};
