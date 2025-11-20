#include "organism.hpp"

Organism::Organism(std::string _name, const OrganismConfig& _organism_config) : Entity(_name), organism_config(_organism_config)
{
	this->stats.vision = _organism_config.vision_radius;
	this->stats.color = _organism_config.color;
	this->stats.hunger = 100;
	this->is_alive = true;

	//add_component(std::make_shared<Behaviour>(weak_from_this(), _organism_config));
	//components.push_back(std::make_shared<Behaviour>(weak_from_this(), _organism_config));
}

void Organism::init()
{
	add_component(std::make_shared<Behaviour>(weak_from_this(), this->organism_config));
    add_component(std::make_shared<SpriteRenderer>(shared_from_this(), stats.color, "being"));
}

Stats& Organism::get_stats()
{
    return stats;
}

Behaviour::Behaviour(std::weak_ptr<Entity> _owner, const OrganismConfig& _organism_config) : Component(_owner), bt(std::make_shared<Node>(nullptr))
{
	this->speed = _organism_config.move_speed;
}

void Behaviour::start()
{
	std::function<BTStatus()> move = [&]()
	{
		if (moving)
		{
			sf::Vector2f pos = owner.lock()->get_transform().get_position();
			sf::Vector2f direction = goal - pos;

			float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

			if (length < 0.001f)
			{
				owner.lock()->get_transform().set_position(goal);
				moving = false;
			}

            float current_effort = 1.0f;
            auto terrain_entity = Scene::instance().get_entity("Terrain");
            std::shared_ptr<EntityTerrain> terrain = nullptr;

            if (terrain_entity)
            {
                terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity);

                if (terrain)
                {
                    sf::Vector2f current_pos = owner.lock()->get_transform().get_position();

                    int pos_x = static_cast<int>(std::floor(current_pos.x));
                    int pos_y = static_cast<int>(std::floor(current_pos.y));

                    current_effort = terrain->get_effort(static_cast<uint16_t>(pos_x), static_cast<uint16_t>(pos_y));
                }
            }

            float effective_speed = speed / current_effort;

			sf::Vector2f dir_normalized = direction / length;

			sf::Vector2f delta = dir_normalized * effective_speed * Time::get_delta();

			if (std::sqrt(delta.x * delta.x + delta.y * delta.y) >= length)
			{
				sf::Vector2f final_goal = goal;
				// Clamp the final goal position to ensure it's within map boundaries
				if (auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity))
				{
					final_goal.x = std::max(0.f, std::min(final_goal.x, (float)terrain->get_width() - 1));
					final_goal.y = std::max(0.f, std::min(final_goal.y, (float)terrain->get_height() - 1));
				}

				owner.lock()->get_transform().set_position(final_goal);
				moving = false;
			}
			else
			{
				sf::Vector2f new_pos = pos + delta;

				// Final safety check: Clamp the new position to map boundaries before translating
				//auto terrain_entity = Scene::instance().get_entity("Terrain");
				if (terrain)
				{
					new_pos.x = std::max(0.f, std::min(new_pos.x, (float)terrain->get_width() - 1));
					new_pos.y = std::max(0.f, std::min(new_pos.y, (float)terrain->get_height() - 1));
				}

				// We set the position directly instead of translating to ensure it's clamped.
				// The difference is negligible for small deltas.
				owner.lock()->get_transform().set_position(new_pos);
			}
			return BTStatus::RUNNING;
		}

		return BTStatus::SUCCESS;
	};

    std::function<BTStatus()> hunger = [&]()
    {
        if (auto organism = std::dynamic_pointer_cast<Organism>(owner.lock())) 
        {
            if (organism->get_stats().hunger > 99)
            {
                return BTStatus::SUCCESS;
            }

			return BTStatus::FAILURE;
        }

        return BTStatus::SUCCESS;
    };

    std::function<BTStatus()> look_food = [&]() {
        auto organism = std::dynamic_pointer_cast<Organism>(owner.lock());
        if (!organism) return BTStatus::FAILURE;

        if (organism->get_stats().hunger >= 99) {
            fixed_entity.reset();
            moving = false;
            return BTStatus::SUCCESS;
        }

        if (fixed_entity && !Scene::instance().has_entity(fixed_entity)) {
            fixed_entity.reset();
            moving = false;
            return BTStatus::RUNNING;
        }

        const sf::Vector2f pos = organism->get_transform().get_position();

        if (!moving && !fixed_entity) {
            static std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> angle_deg_dist(0.0f, 360.0f);
            std::uniform_real_distribution<float> radius_dist(0.0f, 10.0f);

            float angle_deg = angle_deg_dist(rng);
            const float angle_rad = angle_deg * 3.14159265f / 180.0f;
            const float radius = radius_dist(rng);

            sf::Vector2f offset(std::cos(angle_rad) * radius, std::sin(angle_rad) * radius);
            goal = pos + offset;

            auto terrain_entity = Scene::instance().get_entity("Terrain");
            if (auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity))
            {
                float map_w = (float)terrain->get_width();
                float map_h = (float)terrain->get_height();

                // --- LÓGICA DE REBOTE (BOUNCE) ---
                // Si el punto cae fuera, invertimos el offset para que vaya hacia adentro.
                if (goal.x < 0.f || goal.x > map_w - 1.0f ||
                    goal.y < 0.f || goal.y > map_h - 1.0f)
                {
                    offset = -offset; // Invertir dirección
                    goal = pos + offset;
                    
                    // Recalculamos el ángulo para la rotación visual
                    angle_deg = std::atan2(offset.y, offset.x) * 180.0f / 3.14159265f;
                }

                // Clamp final de seguridad (por si acaso)
                goal.x = std::max(0.f, std::min(goal.x, map_w - 1.0f));
                goal.y = std::max(0.f, std::min(goal.y, map_h - 1.0f));
            }

            organism->get_transform().set_rotation(angle_deg);
            moving = true;
            return BTStatus::RUNNING;
        }

        const float eps = 0.001f;
        const sf::Vector2f d = goal - pos;
        const float dist2 = d.x * d.x + d.y * d.y;

        if (dist2 <= eps * eps) {
            if (auto food = std::dynamic_pointer_cast<Food>(fixed_entity)) 
            {

                const sf::Vector2f fpos = food->get_transform().get_position();
                const sf::Vector2f df = fpos - organism->get_transform().get_position();
                if (df.x * df.x + df.y * df.y <= eps * eps) {

                    organism->get_stats().hunger = organism->get_stats().hunger + food->get_nu();
                    Scene::instance().remove_entity(food);
                    fixed_entity.reset();

                    if (organism->get_stats().hunger < 99) {
                        moving = false;
                        return BTStatus::RUNNING;
                    }

                    moving = false;
                    return BTStatus::SUCCESS;
                }
            }
        }

        {
            int vision = organism->get_stats().vision;
            int x_min = std::floor(pos.x - vision);
            int x_max = std::ceil(pos.x + vision);
            int y_min = std::floor(pos.y - vision);
            int y_max = std::ceil(pos.y + vision);

            std::unordered_set<std::shared_ptr<Entity>> visited;
            std::pair<std::shared_ptr<Entity>, float> min_distance = { nullptr, std::numeric_limits<float>::max() };

            for (int i = y_min; i < y_max; i += Constants::chunk_size) {
                for (int j = x_min; j < x_max; j += Constants::chunk_size) {
                    auto chunk_entities = Scene::instance().get_chunk_entities(sf::Vector2f(j, i));
                    for (auto& e : chunk_entities) {
                        if (auto food = std::dynamic_pointer_cast<Food>(e)) {
                            if (visited.insert(food).second) {
                                float dx = food->get_transform().get_position().x - pos.x;
                                float dy = food->get_transform().get_position().y - pos.y;
                                float abs = std::sqrt(dx * dx + dy * dy);
                                if (abs <= vision && abs < min_distance.second) {
                                    min_distance = { food, abs };
                                }
                            }
                        }
                    }
                }
            }

            if (min_distance.first) {
                fixed_entity = min_distance.first;
                goal = fixed_entity->get_transform().get_position();
                sf::Vector2f dir = goal - pos;
                float angle_rad = std::atan2(dir.y, dir.x);
                float angle_deg = angle_rad * 180.f / 3.14159265f;
                organism->get_transform().set_rotation(angle_deg);
                moving = true;
                return BTStatus::RUNNING;
            }
        }

        return BTStatus::RUNNING;
    };


	std::function<BTStatus()> reproduction = [&]()
	{
		auto organism = std::dynamic_pointer_cast<Organism>(owner.lock());
		//std::cout << organism->get_stats().hunger << std::endl;
		return BTStatus::RUNNING;
	};

    std::shared_ptr<Node> root = bt.get_root();
    std::shared_ptr<SequenceNode> selector = std::make_shared<SequenceNode>(root);

	root->add_child(selector);

	std::shared_ptr<ExecutionNode> move_to_target = std::make_shared<ExecutionNode>(selector, move);

    std::shared_ptr<FallbackNode> alimentation_control = std::make_shared<FallbackNode>(selector);

    std::shared_ptr<ExecutionNode> is_hungry = std::make_shared<ExecutionNode>(alimentation_control, hunger);
    std::shared_ptr<ExecutionNode> searh_food = std::make_shared<ExecutionNode>(alimentation_control, look_food);

    alimentation_control->add_child(is_hungry);
    alimentation_control->add_child(searh_food);

    std::shared_ptr<ExecutionNode> reproduce = std::make_shared<ExecutionNode>(selector, reproduction);

	selector->add_child(move_to_target);
	selector->add_child(alimentation_control);
    selector->add_child(reproduce);
}

void Behaviour::update()
{
	bt.tick();

	time += Time::get_delta();

	if (auto organism = std::dynamic_pointer_cast<Organism>(owner.lock()))
	{
		organism->get_stats().hunger -= 5 * Time::get_delta();

		if (organism->get_stats().hunger <= 0)
		{
            std::cout << "Organism " << owner.lock()->get_name() << " has died of starvation." << std::endl;
			Scene::instance().remove_entity(owner.lock());
		}
	}
}

Food::Food(std::string _name) : Entity(_name), nu(50) {}

void Food::init()
{
	add_component(std::make_shared<SpriteRenderer>(shared_from_this(), "fruit"));
}

int Food::get_nu()
{
	return nu;
}

FoodGenerator::FoodGenerator(std::string _name) : Entity(_name) {}

void FoodGenerator::init()
{
	add_component(std::make_shared<FoodSpawner>(shared_from_this()));
}

FoodSpawner::FoodSpawner(std::weak_ptr<Entity> _owner) : Component(_owner) {}

void FoodSpawner::update()
{//
 	time += Time::get_delta();
    if (time >= 5.0f)
    {
	for (int i = 0; i < 20; ++i)
    	{
    		std::shared_ptr<Food> food = EntityFactory<Food>::create("fruit");
    		auto terrain = Scene::instance().get_entity("Terrain"); // Use get_entity for safety
    		uint16_t map_width = terrain ? std::dynamic_pointer_cast<EntityTerrain>(terrain)->get_width() : 100;
    		uint16_t map_height = terrain ? std::dynamic_pointer_cast<EntityTerrain>(terrain)->get_height() : 100;
    		float x = static_cast<float>(std::rand() % map_width);
    		float y = static_cast<float>(std::rand() % map_height);
			food->get_transform().set_position(sf::Vector2f(x, y));
			Scene::instance().add_entity(food);
    	}
    time = 0;
    }
}