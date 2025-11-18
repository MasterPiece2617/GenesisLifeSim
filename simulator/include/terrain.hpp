#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <fstream>

#include "texture.hpp"

struct CellData
{
	uint8_t terrain_type;
	uint8_t texture_id;
	bool is_walkable;
};

class Terrain 
{
protected:
	uint16_t width = 0;
	uint16_t height = 0;
	std::vector<CellData> cells;
	std::shared_ptr<Atlas> atlas;
public:

	Terrain() = default;

	// Load and save terrain data from a binary file
	bool load_from_file(const std::string &filename);

	// manipulate terrain
	void set_cell(uint16_t x, uint16_t y, const CellData& cell);
	CellData get_cell(uint16_t x, uint16_t y) const;

	// getters
	uint16_t get_width() const;
	uint16_t get_height() const;
	bool walkable(uint16_t x, uint16_t y) const;
	
	~Terrain() = default;
};