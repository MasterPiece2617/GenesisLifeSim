#include <scene.hpp>

void Scene::add_entity(std::shared_ptr<Entity> entity)
{
	hierarchy.add_entity(entity);
}

void Scene::start()
{
	for (auto& entity : hierarchy.get_entities())
	{
		entity->start();
	}
}

void Scene::update()
{
	for (auto& entity : hierarchy.get_entities())
	{
		if (entity->get_is_active())
		{
			entity->update();
		}
	}
}