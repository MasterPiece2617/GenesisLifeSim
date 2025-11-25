#include <scene.hpp>

int Scene::organism_id = 0;

sf::Vector2i Scene::quantize(sf::Vector2f pos)
{
	return sf::Vector2i(
		static_cast<int>(pos.x) / Constants::chunk_size,
		static_cast<int>(pos.y) / Constants::chunk_size
	);

}

std::vector<sf::Vector2i> Scene::get_covered_chunks(const sf::Vector2f& center, const sf::Vector2f& scale)
{
	std::vector<sf::Vector2i> covered;

	sf::FloatRect bounds(
		center.x - scale.x * 0.5f,
		center.y - scale.y * 0.5f,
		scale.x,
		scale.y
	);

	int min_x = static_cast<int>(std::floor(bounds.left / Constants::chunk_size));
	int max_x = static_cast<int>(std::floor((bounds.left + bounds.width) / Constants::chunk_size));
	int min_y = static_cast<int>(std::floor(bounds.top / Constants::chunk_size));
	int max_y = static_cast<int>(std::floor((bounds.top + bounds.height) / Constants::chunk_size));

	for (int x = min_x; x <= max_x; ++x)
	{
		for (int y = min_y; y <= max_y; ++y)
		{
			covered.push_back(sf::Vector2i(x, y));
		}
	}

	return covered;
}

Scene::Scene()
{
	main_camera = EntityFactory<Camera>::create("Main Camera");
	add_entity(main_camera);


}

void Scene::add_entity(std::shared_ptr<Entity> entity)
{
	entities.push_back(entity);

	auto& transform = entity->get_transform();
	auto covered_chunks = get_covered_chunks(transform.get_position(), transform.get_scale());

	for (const auto& chunk : covered_chunks)
	{
		chunks[chunk].push_back(entity);
	}

	entity->start();
}

void Scene::spawn_organisms(sf::Vector2f position_meters, OrganismConfig& organism_config)
{
	for (int i = 0; i < num_organisms; ++i)
	{
		auto entity = EntityFactory<Organism>::create("Organism " + std::to_string(++organism_id), organism_config);
		EventManager::publish(Event(EventType::ORGANISM_BORN, EntityEvent(entity)), entity);

		float offset_x = static_cast<float>(std::rand() % 100 - 50) / 100.0f;
		float offset_y = static_cast<float>(std::rand() % 100 - 50) / 100.0f;

		entity->get_transform().set_position({position_meters.x + offset_x, position_meters.y + offset_y});
		add_entity(entity);
	}
}

void Scene::load(const OrganismConfig& organism_config) // Provisional
{
	add_entity(EntityFactory<FoodGenerator>::create("food generator"));
	// auto terrain_entity = Scene::instance().get_entity("Terrain");
    // auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity);

    // uint16_t map_width = terrain ? terrain->get_width() : 100;
    // uint16_t map_height = terrain ? terrain->get_height() : 100;

	// for (int i = 0; i < num_organisms; ++i)
	// {
	// 	auto entity = EntityFactory<Organism>::create("Organism " + std::to_string(i), organism_config);
	// 	EventManager::publish(Event(EventType::ORGANISM_BORN, EntityEvent(entity)), entity);
    //     float final_x = 0.0f;
    //     float final_y = 0.0f;
    //     bool valid_spot = false;

    //     if (terrain)
    //     {
    //         int attempts = 0;
    //         while (attempts < 50 && !valid_spot) 
    //         {
    //             int grid_x = std::rand() % map_width;
    //             int grid_y = std::rand() % map_height;

    //             if (terrain->get_effort(static_cast<uint16_t>(grid_x), static_cast<uint16_t>(grid_y)) == 1.0f)
    //             {
    //                 final_x = (grid_x + 0.5f);
    //                 final_y = (grid_y + 0.5f);
    //                 valid_spot = true;
    //             }
    //             ++attempts;
    //         }
    //     }

	// 	entity->get_transform().set_position({final_x, final_y});
	// 	add_entity(entity);
	// }
}

void Scene::clear()
{
	entities.clear();
	
	for (auto& chunk : chunks)
	{
		chunk.second.clear();
	}

	chunks.clear();

	organism_id = 0;

	main_camera = EntityFactory<Camera>::create("Main Camera");
	add_entity(main_camera);
}

std::shared_ptr<Camera> Scene::get_main_camera() const
{
	return main_camera;
}

std::vector<std::shared_ptr<Entity>> Scene::get_entities() const
{
	return entities;
}

std::shared_ptr<Entity> Scene::get_entity(const std::string& name) const
{
	for (const auto& entity : entities)
	{
		if (entity->get_name() == name)
		{
			return entity;
		}
	}

	return nullptr;
}

bool Scene::has_entity(std::shared_ptr<Entity> entity) const
{
	auto it = std::find(entities.begin(), entities.end(), entity);

	if (it == entities.end())
	{
		return false;
	}

	return true;
}

bool Scene::remove_entity(std::shared_ptr<Entity> entity)
{
	auto it = std::find(entities.begin(), entities.end(), entity);

	if (it == entities.end())
	{
		return false;
	}

	// If the entity is an organism, publish the ORGANISM_DIED event
	if (std::dynamic_pointer_cast<Organism>(entity)) 
	{
        EventManager::publish(Event(EventType::ORGANISM_DIED, EntityEvent(entity)));
    }

	entities.erase(it);

	std::vector<sf::Vector2i> e_chuncks = get_covered_chunks(entity->get_transform().get_position(), entity->get_transform().get_scale());

	for (auto& c : e_chuncks)
	{
		auto it = std::find(chunks[c].begin(), chunks[c].end(), entity);

		if (it != chunks[c].end())
		{
			chunks[c].erase(it);
		}
	}

	entity->get_transform().set_dirty();
	entity->set_delete();

	return true;
}

std::vector<std::shared_ptr<Entity>> Scene::get_chunk_entities(sf::Vector2f coords)
{
	return chunks[quantize(coords)];
}

void Scene::update_entity_grid(std::shared_ptr<Entity> entity, sf::Vector2f old_coords, sf::Vector2f old_scale)
{
	auto old_chunks = get_covered_chunks(old_coords, old_scale);

	for (const auto& chunk : old_chunks)
	{
		auto& vec = chunks[chunk];
		vec.erase(std::remove(vec.begin(), vec.end(), entity), vec.end());
	}

	if (entity->get_delete())
	{
		return;
	}

	auto& transform = entity->get_transform();
	auto new_chunks = get_covered_chunks(transform.get_position(), transform.get_scale());

	for (const auto& chunk : new_chunks)
	{
		chunks[chunk].push_back(entity);
	}
}