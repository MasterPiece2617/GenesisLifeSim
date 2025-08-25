#include <scene.hpp>

Scene::Scene()
{
	main_camera = EntityFactory<Camera>::create("Main Camera");
	add_entity(main_camera);
}

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

	std::shared_ptr<Entity> entity2 = EntityFactory<Entity>::create("Alice");

	sf::CircleShape circle2(30);
	circle2.setFillColor(sf::Color::Red);

	entity2->get_transform().set_position(sf::Vector2f(200, 200));
	entity2->add_component(std::make_shared<Renderer>(entity2, std::make_shared<sf::CircleShape>(circle2)));

	add_entity(entity2);

	std::shared_ptr<Entity> entity3 = EntityFactory<Entity>::create("Charlie");
	sf::CircleShape circle3(20);
	circle3.setFillColor(sf::Color::Green);
	entity3->get_transform().set_position(sf::Vector2f(400, 400));
	entity3->add_component(std::make_shared<Renderer>(entity3, std::make_shared<sf::CircleShape>(circle3)));

	add_entity(entity3);

	std::shared_ptr<Entity> entity4 = EntityFactory<Entity>::create("Eve");
	sf::Sprite sprite;
	std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
	texture->loadFromFile("resources/textures/terrain/grass_floor.png");
	sprite.setTexture(*texture);
	entity4->get_transform().set_position(sf::Vector2f(300, 300));
	entity4->add_component(std::make_shared<Renderer>(entity4, std::make_shared<sf::Sprite>(sprite), texture));
	entity4->get_transform().set_scale(sf::Vector2f(20, 20));
	add_entity(entity4);
}

std::shared_ptr<Camera> Scene::get_main_camera() const
{
	return main_camera;
}

std::vector<std::shared_ptr<Entity>> Scene::get_entities() const
{
	return entities;
}
