#include <scene.hpp>

void Scene::add_entity(std::shared_ptr<Entity> entity)
{
	entities.push_back(entity);
	entity->start();
}

std::vector<std::shared_ptr<Entity>> Scene::get_entities() const
{
	return entities;
}
