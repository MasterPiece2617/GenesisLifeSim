#include <graph.hpp>

bool Hierarchy::add_entity(const std::shared_ptr<Entity> entity)
{
	entities.push_back(entity);
	return true;
}

std::vector<std::shared_ptr<Entity>> Hierarchy::get_entities()
{
	return entities;
}
