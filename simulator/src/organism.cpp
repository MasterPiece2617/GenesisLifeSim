#include "organism.hpp"

Organism::Organism(std::string _name, const OrganismConfig& _organism_config) : Entity(_name), organism_config(_organism_config)
{
	this->stats.vision = _organism_config.vision_radius;
	this->stats.hunger = 100;
	this->is_alive = true;

	//add_component(std::make_shared<Behaviour>(weak_from_this(), _organism_config));
	//components.push_back(std::make_shared<Behaviour>(weak_from_this(), _organism_config));
}

void Organism::init()
{
	components.push_back(std::make_shared<Behaviour>(weak_from_this(), this->organism_config));
    components.push_back(std::make_shared<SpriteRenderer>(shared_from_this(), "being"));
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

			sf::Vector2f dir_normalized = direction / length;
			sf::Vector2f delta = dir_normalized * speed * Time::get_delta();

			if (std::sqrt(delta.x * delta.x + delta.y * delta.y) >= length)
			{
				owner.lock()->get_transform().set_position(goal);
				moving = false;
			}
			else
			{
				owner.lock()->get_transform().translate(delta);
			}
		}

		return BTStatus::SUCCESS;
	};

    std::function<BTStatus()> hunger = [&]()
    {
        if (auto organism = std::dynamic_pointer_cast<Organism>(owner.lock())) 
        {
            if (organism->get_stats().hunger < 99)
            {
				std::cout << "hungry\n";
                return BTStatus::FAILURE;
            }
        }

        return BTStatus::SUCCESS;
    };

    std::function<BTStatus()> look_food = [&]()
    {
		if (fixed_entity && !Scene::instance().has_entity(fixed_entity))
		{
			fixed_entity.reset();
			moving = false;
			return BTStatus::RUNNING;
		}

		sf::Vector2f pos = owner.lock()->get_transform().get_position();

		if (!moving && !fixed_entity)
		{
			static std::mt19937 rng(std::random_device{}());
			std::uniform_real_distribution<float> angle_deg_dist(0.0f, 360.0f);
			std::uniform_real_distribution<float> radius_dist(0.0f, 10.0f);

			float angle_deg = angle_deg_dist(rng);
			float angle_rad = angle_deg * 3.14159265f / 180.0f;
			float radius = radius_dist(rng);

			sf::Vector2f offset(std::cos(angle_rad) * radius, std::sin(angle_rad) * radius);
			goal = pos + offset;

			owner.lock()->get_transform().set_rotation(angle_deg);
			moving = true;

			return BTStatus::RUNNING;
		}
		else if (pos == goal)
		{
			std::shared_ptr<Food> food = std::dynamic_pointer_cast<Food>(fixed_entity);

			if (food)
			{
				if (fixed_entity->get_transform().get_position() == owner.lock()->get_transform().get_position())
				{
					std::shared_ptr<Organism> entity = std::dynamic_pointer_cast<Organism>(owner.lock());

					if (entity)
					{
						entity->get_stats().hunger += food->get_nu();
						Scene::instance().remove_entity(food);
						fixed_entity.reset();
					}
				}
			}
		}

		std::shared_ptr<Organism> entity = std::dynamic_pointer_cast<Organism>(owner.lock());

		int x_min = std::floor(pos.x - entity->get_stats().vision);
		int x_max = std::ceil(pos.x + entity->get_stats().vision);
		int y_min = std::floor(pos.y - entity->get_stats().vision);
		int y_max = std::ceil(pos.y + entity->get_stats().vision);

		std::unordered_set<std::shared_ptr<Entity>> visited;
		std::pair<std::shared_ptr<Entity>, float> min_distance = { nullptr, std::numeric_limits<float>::max() };

		for (int i = y_min; i < y_max; i += Constants::chunk_size)
		{
			for (int j = x_min; j < x_max; j += Constants::chunk_size)
			{
				std::vector<std::shared_ptr<Entity>> chunk_entities = Scene::instance().get_chunk_entities(sf::Vector2f(j, i));

				for (auto& e : chunk_entities)
				{
					std::shared_ptr<Food> food = std::dynamic_pointer_cast<Food>(e);

					if (food)
					{
						if (visited.find(food) == visited.end())
						{
							visited.insert(food);

							float abs = std::sqrt(std::pow(food->get_transform().get_position().x - pos.x, 2) + std::pow(food->get_transform().get_position().y - pos.y, 2));

							if (abs <= entity->get_stats().vision)
							{
								if (min_distance.second > abs)
								{
									min_distance = { food, abs };
								}
							}
						}
					}
				}
			}
		}

		if (!min_distance.first)
		{
			return BTStatus::RUNNING;
		}

		fixed_entity = min_distance.first;
		goal = min_distance.first->get_transform().get_position();
		sf::Vector2f dir = goal - pos;
		float angle_rad = std::atan2(dir.y, dir.x);
		float angle_deg = angle_rad * 180.f / std::_Pi_val;
		entity->get_transform().set_rotation(angle_deg);

		return BTStatus::SUCCESS;
    };

	std::function<BTStatus()> reproduction = [&]()
	{
		auto organism = std::dynamic_pointer_cast<Organism>(owner.lock());
		std::cout << organism->get_stats().hunger << std::endl;
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
	/*
	std::cout << root << " root \n";
	std::cout << selector << " selector \n";
	std::cout << alimentation_control << " alimentation_control \n";
	std::cout << is_hungry << " is_hungry \n";
	std::cout << searh_food << " searh_food \n";
	std::cout << reproduce << " reproduce \n";*/
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
{
	time += Time::get_delta();

	if (time >= 5.0f)
	{
		
		for (int i = 0; i < 20; ++i)
		{
			std::shared_ptr<Food> food = EntityFactory<Food>::create("fruit");

			float x = static_cast<float>(std::rand() % 121 - 60);
			float y = static_cast<float>(std::rand() % 121 - 60);

			food->get_transform().set_position(sf::Vector2f(x, y));


			Scene::instance().add_entity(food);
		}

		time = 0;
	}
}