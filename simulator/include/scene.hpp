#pragma once

#include <entity_test.hpp>
#include <component_test.hpp>
#include <graph.hpp>

class Scene
{
protected:
	Hierarchy hierarchy;

public:
	Scene() = default; // Provisional
	void add_entity(std::shared_ptr<Entity> entity); // Provisional
	void start();
	void update();
	~Scene() = default; 
};