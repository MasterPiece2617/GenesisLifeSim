#pragma once

#include <unordered_map>
#include <memory>

#include <entity_test.hpp>
#include <camera.hpp>
#include <component_test.hpp>
#include <graph.hpp>
#include <organism.hpp>
#include <renderer.hpp>
#include <utils.hpp>
#include <organism_config.hpp>

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
	std::vector<sf::Vector2i> get_covered_chunks(const sf::Vector2f& center, const sf::Vector2f& scale);

public:

	int num_organisms = 5;
	int tree_spawn = 0;
	static int organism_id;

	Scene(); 
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	static Scene& instance() {
		static Scene instance;
		return instance;
	}

	void add_entity(std::shared_ptr<Entity> entity);
	void spawn_organisms(sf::Vector2f position_meters, OrganismConfig& organism_config);
	void load(const OrganismConfig& organism_config);
	void clear();
	std::shared_ptr<Camera> get_main_camera() const;
	std::vector<std::shared_ptr<Entity>> get_entities() const;
	std::shared_ptr<Entity> get_entity(const std::string& name) const;
	bool has_entity(std::shared_ptr<Entity> entity) const;
	bool remove_entity(std::shared_ptr<Entity> entity);
	std::vector<std::shared_ptr<Entity>> get_chunk_entities(sf::Vector2f coords);
	void update_entity_grid(std::shared_ptr<Entity> entity, sf::Vector2f old_coords, sf::Vector2f old_scale);

	~Scene() = default; 
};