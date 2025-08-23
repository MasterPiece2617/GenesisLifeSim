#pragma once

#include <entity_test.hpp>
#include <component_test.hpp>
#include <graph.hpp>

class Scene
{

protected:

	std::vector<std::shared_ptr<Entity>> entities;

public:

	Scene() = default; // Provisional
	void add_entity(std::shared_ptr<Entity> entity);
	std::vector<std::shared_ptr<Entity>> get_entities() const;
	~Scene() = default; 
};