#pragma once

#include <string>
#include <cstdint>
#include <fstream>
#include <vector>
#include <iostream>

#define FASTNOISE_LITE_IMPLEMENTATION
#include <FastNoiseLite.h>

struct CellData
{
	uint8_t terrain_type;
	uint8_t texture_id;
	uint8_t effort;
	bool is_walkable;
    bool is_navigable;
    bool is_plantable;
};

class MapGenerator
{
public:
    struct Config
    {
        uint16_t width = 200;
        uint16_t height = 200;
        int seed = 1337;
        float frequency = 0.02f;
        int octaves = 5;
        float detail_freq = 0.05f;
        std::string filename = "custom_map.zadat";
    };

    static void generate_map(const Config& config);
};