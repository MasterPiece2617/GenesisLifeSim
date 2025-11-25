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
	uint8_t effort;
	bool is_walkable;
	bool is_navigable;
	bool is_plantable;
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
	float get_effort(uint16_t x, uint16_t y) const;
	bool navigable(uint16_t x, uint16_t y) const;
	bool plantable(uint16_t x, uint16_t y) const;
	
	~Terrain() = default;
};