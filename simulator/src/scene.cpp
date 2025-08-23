#include <scene.hpp>

void Scene::add_entity(std::shared_ptr<Entity> entity)
{
	entities.push_back(entity);
	entity->start();
}

void Scene::load() // Provisional
{
	std::shared_ptr<Entity> entity1 = EntityFactory<Entity>::create("Bobby");

	sf::CircleShape circle(50);
	circle.setFillColor(sf::Color::Blue);

	entity1->add_component(std::make_shared<Renderer>(entity1, std::make_shared<sf::CircleShape>(circle)));
	add_entity(entity1);
	std::cout << "Entity " << entity1->get_name() << " created." << std::endl;
}

std::vector<std::shared_ptr<Entity>> Scene::get_entities() const
{
	return entities;
}
