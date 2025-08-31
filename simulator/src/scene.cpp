#include <scene.hpp>

sf::Vector2i Scene::quantize(sf::Vector2f pos)
{
	return sf::Vector2i(
		static_cast<int>(pos.x) / Constants::chunk_size,
		static_cast<int>(pos.y) / Constants::chunk_size
	);

}

Scene::Scene()
{
	main_camera = EntityFactory<Camera>::create("Main Camera");
	add_entity(main_camera);


}

void Scene::add_entity(std::shared_ptr<Entity> entity)
{
	entities.push_back(entity);
	chunks[quantize(entity->get_transform().get_position())].push_back(entity);
	entity->start();
}

void Scene::load() // Provisional
{
	for (int i = 0; i < 10; ++i)
	{
		std::shared_ptr<Entity> entity = EntityFactory<Entity>::create("Entity " + std::to_string(i));
		entity->get_transform().set_position(sf::Vector2f(i / 256, i % 256));
		entity->add_component(std::make_shared<SpriteRenderer>(entity, "being"));

		add_entity(entity);
	}
}

std::shared_ptr<Camera> Scene::get_main_camera() const
{
	return main_camera;
}

std::vector<std::shared_ptr<Entity>> Scene::get_entities() const
{
	return entities;
}

std::vector<std::shared_ptr<Entity>> Scene::get_chunk_entities(sf::Vector2f coords)
{
	return chunks[quantize(coords)];
}

void Scene::update_entity_grid(std::shared_ptr<Entity> entity, sf::Vector2f old_coords)
{
	sf::Vector2f coords = entity->get_transform().get_position();
	std::vector<std::shared_ptr<Entity>>& entities_vector = chunks[quantize(old_coords)];

	auto it = std::find(entities_vector.begin(), entities_vector.end(), entity);

	if (it != entities_vector.end())
	{
		entities_vector.erase(it);
	}

	chunks[quantize(coords)].push_back(entity);
}