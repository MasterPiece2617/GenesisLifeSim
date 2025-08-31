#pragma once

#include <entity_test.hpp>
#include <camera.hpp>
#include <component_test.hpp>
#include <graph.hpp>
#include <renderer.hpp>
#include <utils.hpp>

#include <unordered_map>

struct ChunkHash {
	std::size_t operator()(const sf::Vector2i& p) const {
		std::size_t h1 = std::hash<int>()(p.x);
		std::size_t h2 = std::hash<int>()(p.y);
		return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
	}
};

using ChunkGrid = std::unordered_map<sf::Vector2i, std::vector<std::shared_ptr<Entity>>, ChunkHash>;

class Scene
{

protected:

	std::vector<std::shared_ptr<Entity>> entities;
	std::shared_ptr<Camera> main_camera;
	ChunkGrid chunks;

	sf::Vector2i quantize(sf::Vector2f key);

public:

	Scene(); // Provisional
	void add_entity(std::shared_ptr<Entity> entity);
	void load(); // Provisional
	std::shared_ptr<Camera> get_main_camera() const;
	std::vector<std::shared_ptr<Entity>> get_entities() const;
	std::vector<std::shared_ptr<Entity>> get_chunk_entities(sf::Vector2f coords);
	void update_entity_grid(std::shared_ptr<Entity> entity, sf::Vector2f old_coords);

	~Scene() = default; 
};