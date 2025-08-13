#pragma once

#include <entity_test.hpp>
#include <component_test.hpp>
#include <graph.hpp>

class Scenario
{
protected:
	Hierarchy hierarchy; // Esto se va a cambiar por un grafo/árbol. Asignación de José...

public:
	Scenario() = default; // Provisional
	void add_entity(std::shared_ptr<Entity> entity); // Provisional
	void start();
	void update();
	~Scenario() = default; 
};