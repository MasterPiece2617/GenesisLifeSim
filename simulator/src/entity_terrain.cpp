#include <entity_terrain.hpp>

EntityTerrain::EntityTerrain(const std::string& map_from_file, const std::shared_ptr<Atlas>& _atlas)
	: Entity("Terrain"), atlas(_atlas) 
{
	if (terrain.load_from_file(map_from_file))
	{
		generate_vertices();
	}
	else
	{
		throw std::runtime_error("Failed to load terrain from file: " + map_from_file);
	}
}

//map image data with texture id
std::unordered_map<uint16_t, std::string> texture_id_map = {
	{1, "grass_floor"},
	{2, "water_floor"},
};

void EntityTerrain::generate_vertices()
{
	vertices.setPrimitiveType(sf::Quads);
	vertices.resize(terrain.get_width() * terrain.get_height() * 4);

	for (uint16_t y = 0; y < terrain.get_height(); ++y)
	{
		for (uint16_t x = 0; x < terrain.get_width(); ++x)
		{
			CellData cell = terrain.get_cell(x, y);

			// Get the texture coordinates from the atlas
			sf::IntRect texture_rect = atlas->get_regions().at(texture_id_map[terrain.get_cell(x, y).texture_id]);

			size_t quad_index = (y * terrain.get_width() + x) * 4;

			vertices[quad_index + 0].position = sf::Vector2f(x * Constants::px_mt, y * Constants::px_mt);
			vertices[quad_index + 1].position = sf::Vector2f((x + 1) * Constants::px_mt, y * Constants::px_mt);
			vertices[quad_index + 2].position = sf::Vector2f((x + 1) * Constants::px_mt, (y + 1) * Constants::px_mt);
			vertices[quad_index + 3].position = sf::Vector2f(x * Constants::px_mt, (y + 1) * Constants::px_mt);

			// Asigna las coordenadas de textura
			vertices[quad_index + 0].texCoords = sf::Vector2f(texture_rect.left, texture_rect.top);
			vertices[quad_index + 1].texCoords = sf::Vector2f(texture_rect.left + texture_rect.width, texture_rect.top);
			vertices[quad_index + 2].texCoords = sf::Vector2f(texture_rect.left + texture_rect.width, texture_rect.top + texture_rect.height);
			vertices[quad_index + 3].texCoords = sf::Vector2f(texture_rect.left, texture_rect.top + texture_rect.height);
		}
	}
}

void EntityTerrain::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// Apply the texture
	states.texture = &atlas->get_atlas();
	// Draw the vertex array
	target.draw(vertices, states);
}

// terrain data methods
bool EntityTerrain::walkable(uint16_t x, uint16_t y) const
{
	return terrain.walkable(x, y);
}

void EntityTerrain::set_cell(uint16_t x, uint16_t y, const CellData& cell)
{
	terrain.set_cell(x, y, cell);

	// Update the corresponding vertices
	sf::IntRect texture_rect = atlas->get_regions().at(texture_id_map[terrain.get_cell(x, y).texture_id]);
	size_t quad_index = (y * terrain.get_width() + x) * Constants::chunk_size;

	vertices[quad_index + 0].texCoords = sf::Vector2f(texture_rect.left, texture_rect.top);
	vertices[quad_index + 1].texCoords = sf::Vector2f(texture_rect.left + texture_rect.width, texture_rect.top);
	vertices[quad_index + 2].texCoords = sf::Vector2f(texture_rect.left + texture_rect.width, texture_rect.top + texture_rect.height);
	vertices[quad_index + 3].texCoords = sf::Vector2f(texture_rect.left, texture_rect.top + texture_rect.height);
}

CellData EntityTerrain::get_cell(uint16_t x, uint16_t y) const
{
	return terrain.get_cell(x, y);
}

uint16_t EntityTerrain::get_width() const
{
	return terrain.get_width();
}

uint16_t EntityTerrain::get_height() const
{
	return terrain.get_height();
}