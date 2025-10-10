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
	void load();
	std::shared_ptr<Camera> get_main_camera() const;
	std::vector<std::shared_ptr<Entity>> get_entities() const;
	bool has_entity(std::shared_ptr<Entity> entity) const;
	bool remove_entity(std::shared_ptr<Entity> entity);
	std::vector<std::shared_ptr<Entity>> get_chunk_entities(sf::Vector2f coords);
	void update_entity_grid(std::shared_ptr<Entity> entity, sf::Vector2f old_coords, sf::Vector2f old_scale);

	~Scene() = default; 
};