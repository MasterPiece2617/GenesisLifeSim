#pragma once

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <entity.hpp>
#include <terrain.hpp>
#include <renderer.hpp>

class EntityTerrain : public Entity
{
protected:
	Terrain terrain;
	sf::VertexArray vertices;
	std::shared_ptr<Atlas> atlas;

	void generate_vertices();
public:
	EntityTerrain(const std::string& map_from_file, const std::shared_ptr<Atlas>& atlas);
	
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	// methods with terrain data
	bool walkable(uint16_t x, uint16_t y) const;
	void set_cell(uint16_t x, uint16_t y, const CellData& cell);
	uint16_t get_width() const;
	uint16_t get_height() const;

	~EntityTerrain() = default;
};