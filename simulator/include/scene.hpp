#pragma once

#include <entity_test.hpp>
#include <camera.hpp>
#include <component_test.hpp>
#include <graph.hpp>
#include <renderer.hpp>

class Scene
{

protected:

	std::vector<std::shared_ptr<Entity>> entities;
	std::shared_ptr<Camera> main_camera;

public:

	Scene(); // Provisional
	void add_entity(std::shared_ptr<Entity> entity);
	void load(); // Provisional
	std::shared_ptr<Camera> get_main_camera() const;
	std::vector<std::shared_ptr<Entity>> get_entities() const;
	~Scene() = default; 
};