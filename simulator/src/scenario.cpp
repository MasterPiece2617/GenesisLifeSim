#include <scenario.hpp>

void Scenario::add_entity(std::shared_ptr<Entity> entity)
{
	hierarchy.add_entity(entity);
}

void Scenario::start()
{
	for (auto& entity : hierarchy.get_entities())
	{
		entity->start();
	}
}

void Scenario::update()
{
	for (auto& entity : hierarchy.get_entities())
	{
		entity->update();
	}
}