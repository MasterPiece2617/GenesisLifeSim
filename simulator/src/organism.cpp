#include "organism.hpp"

Organism::Organism(std::string _name, const Stats _stats) : Entity(_name), stats(_stats) {}

Organism::Organism(std::string _name, const OrganismConfig& _organism_config) : Entity(_name)
{
	stats.vision = _organism_config.vision_radius;
	stats.color = _organism_config.color;
	stats.hunger = _organism_config.hunger;
	stats.category = _organism_config.category;
	stats.speed = _organism_config.move_speed;
	stats.hp = _organism_config.hp;
	stats.size = _organism_config.size;
	stats.weight = _organism_config.weight;
	stats.stamina = _organism_config.stamina;
	stats.nu = _organism_config.nu;
	stats.max_hp = _organism_config.max_hp;
	stats.max_hunger = _organism_config.max_hunger;
	stats.max_stamina = _organism_config.max_stamina;
	is_alive = true;

	//add_component(std::make_shared<Behaviour>(weak_from_this(), _organism_config));
	//components.push_back(std::make_shared<Behaviour>(weak_from_this(), _organism_config));
}

bool Organism::get_is_resting()
{
	return is_resting;
}

void Organism::set_is_resting(bool rest)
{
	is_resting = rest;
}

void Organism::init()
{
	add_component(std::make_shared<Behaviour>(weak_from_this()));
	add_component(std::make_shared<SpriteRenderer>(shared_from_this(), stats.color, "being"));

	transform->set_scale({ stats.size, stats.size });
}

std::string Organism::get_state()
{
	return state;
}

void Organism::set_state(std::string new_state)
{
	state = new_state;
}

Stats& Organism::get_stats()
{
	return stats;
}

Behaviour::Behaviour(std::weak_ptr<Entity> _owner) : Component(_owner), bt(std::make_shared<Node>(nullptr)) {}

void Behaviour::start()
{
	std::function<BTStatus()> move = [&]()
		{
			if (moving)
			{
				auto organism = std::dynamic_pointer_cast<Organism>(owner.lock());
				if (!organism) return BTStatus::FAILURE;

				Stats& s = organism->get_stats();

				// --- DESCANSO SI NO TIENE STAMINA ---
				if (s.stamina <= 0.0f || organism->get_is_resting())
				{
					// Recuperación: 100 stamina/s drenando hunger
					float recover = 100.0f * Time::get_delta() * Time::get_simulation_speed();
					float hunger_cost = recover / 10.0f; // 1 hunger -> 10 stamina
					organism->set_is_resting(true);

					if (s.hunger > hunger_cost)
					{
						s.hunger -= hunger_cost;
						s.stamina = std::min(s.stamina + recover, s.max_stamina);
					}

					if (s.stamina >= s.max_stamina)
					{
						organism->set_is_resting(false);
					}

					// No se mueve mientras descansa
					return BTStatus::RUNNING;
				}

				// --- MOVIMIENTO NORMAL ---
				sf::Vector2f pos = organism->get_transform().get_position();
				sf::Vector2f current_target = goal;
				bool using_path = !path.empty();

				if (fixed_entity)
				{
					sf::Vector2f fixed_pos = fixed_entity->get_transform().get_position();
					if (fixed_pos != goal) goal = fixed_pos;
				}
				if (using_path)
				{
					if (path_index > path.size() - 1) {
						moving = false;
						path.clear();
						return BTStatus::SUCCESS;
					}
					current_target = path[path_index];
				}

				sf::Vector2f direction = current_target - pos;
				float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

				if (length < 0.1f)
				{
					organism->get_transform().set_position(current_target);
					if (using_path) {
						++path_index;
						if (path_index < path.size()) return BTStatus::RUNNING;
						path.clear();
					}
					moving = false;
					return BTStatus::SUCCESS;
				}

				// --- TERRENO ---
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
						current_effort = terrain->get_effort((uint16_t)pos_x, (uint16_t)pos_y);
					}
				}

				float effective_speed = s.speed / current_effort;
				sf::Vector2f dir_normalized = direction / length;
				sf::Vector2f delta = dir_normalized * effective_speed * Time::get_delta() * Time::get_simulation_speed();

				// --- GASTO DE STAMINA ---
				float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
				float stamina_cost = distance * (0.5f * s.speed + 0.3f * s.size);
				s.stamina = std::max(0.0f, s.stamina - stamina_cost);

				// Si se quedó sin stamina, se detiene
				if (s.stamina <= 0.0f)
				{
					moving = false;
					return BTStatus::RUNNING;
				}

				// --- AVANCE ---
				if (distance >= length)
				{
					sf::Vector2f final_pos = current_target;
					if (terrain)
					{
						final_pos.x = std::max(0.f, std::min(final_pos.x, (float)terrain->get_width() - 1));
						final_pos.y = std::max(0.f, std::min(final_pos.y, (float)terrain->get_height() - 1));
					}
					owner.lock()->get_transform().set_position(final_pos);

					if (using_path) {
						++path_index;
						if (path_index < path.size()) return BTStatus::RUNNING;
						path.clear();
					}
					moving = false;
				}
				else
				{
					sf::Vector2f new_pos = pos + delta;
					if (terrain)
					{
						new_pos.x = std::max(0.f, std::min(new_pos.x, (float)terrain->get_width() - 1));
						new_pos.y = std::max(0.f, std::min(new_pos.y, (float)terrain->get_height() - 1));
					}
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
				if (organism->get_stats().hunger > (organism->get_stats().max_hunger * 0.9f))
				{
					return BTStatus::SUCCESS;
				}
				organism->set_state("Hungry");
				return BTStatus::FAILURE;
			}

			return BTStatus::SUCCESS;
		};

	std::function<BTStatus()> look_food = [&]() {

		auto organism = std::dynamic_pointer_cast<Organism>(owner.lock());
		if (!organism) return BTStatus::FAILURE;

		if (organism->get_stats().hunger >= (organism->get_stats().max_hunger * 0.9f))
		{
			fixed_entity.reset();
			moving = false;
			return BTStatus::SUCCESS;
		}

		if (fixed_entity && !Scene::instance().has_entity(fixed_entity))
		{
			fixed_entity.reset();
			moving = false;
			return BTStatus::RUNNING;
		}

		const sf::Vector2f pos = organism->get_transform().get_position();

		if (!moving && !fixed_entity)
		{
			static std::mt19937 rng(std::random_device{}());
			std::uniform_real_distribution<float> angle_deg_dist(0.0f, 360.0f);
			std::uniform_real_distribution<float> radius_dist(0.0f, 10.0f);

			float angle_deg = angle_deg_dist(rng);
			const float angle_rad = angle_deg * Constants::pi_val / 180.0f;
			const float radius = radius_dist(rng);

			sf::Vector2f offset(std::cos(angle_rad) * radius, std::sin(angle_rad) * radius);
			goal = pos + offset;

			auto terrain_entity = Scene::instance().get_entity("Terrain");
			if (auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity))
			{
				float map_w = (float)terrain->get_width();
				float map_h = (float)terrain->get_height();

				if (goal.x < 0.f || goal.x > map_w - 1.0f ||
					goal.y < 0.f || goal.y > map_h - 1.0f ||
					(!terrain->navigable(static_cast<uint16_t>(goal.x), static_cast<uint16_t>(goal.y)) &&
					 !terrain->walkable(static_cast<uint16_t>(goal.x), static_cast<uint16_t>(goal.y))))
				{
					return BTStatus::FAILURE;
				}

				// Clamp final de seguridad (por si acaso)
				goal.x = std::max(0.f, std::min(goal.x, map_w - 1.0f));
				goal.y = std::max(0.f, std::min(goal.y, map_h - 1.0f));
			}

			organism->get_transform().set_rotation(angle_deg);
			moving = true;
			return BTStatus::RUNNING;
		}

		const float eps = 0.005f;
		const sf::Vector2f d = goal - pos;
		const float dist2 = d.x * d.x + d.y * d.y;

		if (organism->get_stats().category == OrganismCategory::HERBIVORE)
		{
			if (dist2 <= eps * eps)
			{
				if (auto food = std::dynamic_pointer_cast<Food>(fixed_entity))
				{

					const sf::Vector2f fpos = food->get_transform().get_position();
					const sf::Vector2f df = fpos - organism->get_transform().get_position();
					if (df.x * df.x + df.y * df.y <= eps * eps) {

						organism->get_stats().hunger = std::min(organism->get_stats().hunger + food->get_nu(), organism->get_stats().max_hunger);
						Scene::instance().remove_entity(food);
						fixed_entity.reset();

						if (organism->get_stats().hunger < (organism->get_stats().max_hunger * 0.9f)) {
							moving = false;
							return BTStatus::RUNNING;
						}

						moving = false;
						return BTStatus::SUCCESS;
					}
				}
			}

			if (fixed_entity)
			{
				return BTStatus::RUNNING;
			}

			organism->set_state("Searching Food");

			int vision = organism->get_stats().vision;
			int x_min = std::floor(pos.x - vision);
			int x_max = std::ceil(pos.x + vision);
			int y_min = std::floor(pos.y - vision);
			int y_max = std::ceil(pos.y + vision);

			std::unordered_set<std::shared_ptr<Entity>> visited;
			std::pair<std::shared_ptr<Entity>, float> min_distance = { nullptr, std::numeric_limits<float>::max() };

			for (int i = y_min; i < y_max; i += Constants::chunk_size)
			{
				for (int j = x_min; j < x_max; j += Constants::chunk_size)
				{
					auto chunk_entities = Scene::instance().get_chunk_entities(sf::Vector2f(j, i));

					for (auto& e : chunk_entities)
					{
						if (auto food = std::dynamic_pointer_cast<Food>(e))
						{
							if (visited.insert(food).second)
							{
								float dx = food->get_transform().get_position().x - pos.x;
								float dy = food->get_transform().get_position().y - pos.y;
								float abs = std::sqrt(dx * dx + dy * dy);

								if (abs <= vision && abs < min_distance.second)
								{
									organism->set_state("Going to Food");
									min_distance = { food, abs };
								}
							}
						}
					}
				}
			}

			if (min_distance.first)
			{
				fixed_entity = min_distance.first;
				goal = fixed_entity->get_transform().get_position();
				sf::Vector2f dir = goal - pos;
				float angle_rad = std::atan2(dir.y, dir.x);
				float angle_deg = angle_rad * 180.f / Constants::pi_val;
				organism->get_transform().set_rotation(angle_deg);

				auto terrain_entity = Scene::instance().get_entity("Terrain");
				if (auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity))
				{
					// 1. Calculamos la ruta
					path = pathfinder.find_path(pos, goal, *terrain);

					// 2. Si A* encontró un camino válido
					if (!path.empty())
					{
						path_index = 0;

						// saltamos al segundo para que el movimiento arranque fluido.
						if (path.size() > 1)
						{
							sf::Vector2f p0 = path[0];
							float d2 = (p0.x - pos.x) * (p0.x - pos.x) + (p0.y - pos.y) * (p0.y - pos.y);
							if (d2 < 100.0f)
							{
								// Si está a menos de ~10 pixeles
								path_index = 1;
							}
						}

						moving = true;
						return BTStatus::RUNNING;
					}
				}
			}

			return BTStatus::RUNNING;
		}
		else if (organism->get_stats().category == OrganismCategory::CARNIVORE)
		{
			// --- Caso: ya alcanzó la meta ---
			if (dist2 <= eps * eps)
			{
				if (auto prey = std::dynamic_pointer_cast<Organism>(fixed_entity))
				{
					const sf::Vector2f ppos = prey->get_transform().get_position();
					const sf::Vector2f dp = ppos - organism->get_transform().get_position();
					if (dp.x * dp.x + dp.y * dp.y <= eps * eps)
					{
						// "Comer" al herbívoro
						organism->get_stats().hunger = std::min(organism->get_stats().hunger + prey->get_stats().nu, organism->get_stats().max_hunger);
						Scene::instance().remove_entity(prey);
						fixed_entity.reset();

						if (organism->get_stats().hunger < (organism->get_stats().max_hunger * 0.9f)) {
							moving = false;
							return BTStatus::RUNNING;
						}

						moving = false;
						return BTStatus::SUCCESS;
					}
				}
			}
			if (fixed_entity) {
				if (Scene::instance().has_entity(fixed_entity)) {
					// Actualizar meta
					goal = fixed_entity->get_transform().get_position();

					// Recalcular dirección y rotación visual
					auto organism = std::dynamic_pointer_cast<Organism>(owner.lock());
					if (organism) {
						sf::Vector2f pos = organism->get_transform().get_position();
						sf::Vector2f dir = goal - pos;
						float angle_rad = std::atan2(dir.y, dir.x);
						float angle_deg = angle_rad * 180.f / Constants::pi_val;
						organism->get_transform().set_rotation(angle_deg);
					}

					// Asegurar que siga en modo persecución
					moving = true;

					return BTStatus::RUNNING;
				}
				else {
					fixed_entity.reset();
					moving = false;
				}
			}

			int vision = organism->get_stats().vision;
			int x_min = std::floor(pos.x - vision);
			int x_max = std::ceil(pos.x + vision);
			int y_min = std::floor(pos.y - vision);
			int y_max = std::ceil(pos.y + vision);

			std::unordered_set<std::shared_ptr<Entity>> visited;
			std::pair<std::shared_ptr<Entity>, float> min_distance = { nullptr, std::numeric_limits<float>::max() };

			for (int i = y_min; i < y_max; i += Constants::chunk_size)
			{
				for (int j = x_min; j < x_max; j += Constants::chunk_size)
				{
					auto chunk_entities = Scene::instance().get_chunk_entities(sf::Vector2f(j, i));

					for (auto& e : chunk_entities)
					{
						if (auto prey = std::dynamic_pointer_cast<Organism>(e))
						{
							if (prey->get_stats().category == OrganismCategory::HERBIVORE)
							{
								if (visited.insert(prey).second)
								{
									float dx = prey->get_transform().get_position().x - pos.x;
									float dy = prey->get_transform().get_position().y - pos.y;
									float abs = std::sqrt(dx * dx + dy * dy);

									if (abs <= vision && abs < min_distance.second)
									{
										min_distance = { prey, abs };
									}
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
				float angle_deg = angle_rad * 180.f / Constants::pi_val;
				organism->get_transform().set_rotation(angle_deg);

				auto terrain_entity = Scene::instance().get_entity("Terrain");
				if (auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity))
				{
					path = pathfinder.find_path(pos, goal, *terrain);
					if (!path.empty())
					{
						path_index = 0;
						if (path.size() > 1)
						{
							sf::Vector2f p0 = path[0];
							float d2 = (p0.x - pos.x) * (p0.x - pos.x) + (p0.y - pos.y) * (p0.y - pos.y);
							if (d2 < 100.0f)
							{
								path_index = 1;
							}
						}
						moving = true;
						return BTStatus::RUNNING;
					}
				}
			}

			return BTStatus::RUNNING;
		}
		};


	std::function<BTStatus()> reproduction = [&]()
		{
			auto owner_sp = owner.lock();
			if (!owner_sp) return BTStatus::FAILURE;

			auto og = std::dynamic_pointer_cast<Organism>(owner_sp);
			if (!og) return BTStatus::RUNNING;

			const sf::Vector2f pos = og->get_transform().get_position();

			static std::unordered_map<const Organism*, int> cooldown_ticks;

			const int COOLDOWN_TICKS = 300;

			static std::mt19937 rng(std::random_device{}());
			std::uniform_real_distribution<float> prob_dist(0.0f, 1.0f);
			const float REPRODUCE_PROB = 0.05f;

			{
				auto it = cooldown_ticks.find(og.get());
				if (it != cooldown_ticks.end() && it->second > 0) it->second--;
			}

			int vision = og->get_stats().vision;
			int x_min = std::floor(pos.x - vision);
			int x_max = std::ceil(pos.x + vision);
			int y_min = std::floor(pos.y - vision);
			int y_max = std::ceil(pos.y + vision);

			std::unordered_set<std::shared_ptr<Entity>> visited;
			std::shared_ptr<Organism> nearest = nullptr;
			float min_dist = std::numeric_limits<float>::max();

			for (int i = y_min; i < y_max; i += Constants::chunk_size)
			{
				for (int j = x_min; j < x_max; j += Constants::chunk_size)
				{
					auto chunk_entities = Scene::instance().get_chunk_entities(sf::Vector2f(j, i));
					for (auto& e : chunk_entities)
					{
						if (e == owner_sp) continue;

						if (auto other_og = std::dynamic_pointer_cast<Organism>(e))
						{
							if (!visited.insert(other_og).second) continue;
							if (other_og->get_stats().category != og->get_stats().category) continue;

							float dx = other_og->get_transform().get_position().x - pos.x;
							float dy = other_og->get_transform().get_position().y - pos.y;
							float dist = std::sqrt(dx * dx + dy * dy);

							if (dist <= vision && dist < min_dist)
							{
								min_dist = dist;
								nearest = other_og;
							}
						}
					}
				}
			}

			const float reproduce_range = 1.0f;

			if (nearest)
			{
				{
					auto it = cooldown_ticks.find(nearest.get());
					if (it != cooldown_ticks.end() && it->second > 0) it->second--;
				}

				if (min_dist <= reproduce_range)
				{
					const int my_cd = cooldown_ticks[og.get()];
					const int other_cd = cooldown_ticks[nearest.get()];
					const bool can_reproduce = (my_cd <= 0) && (other_cd <= 0);

					const bool enough_hunger = (og->get_stats().hunger >= og->get_stats().max_hunger * 0.3f) && (nearest->get_stats().hunger >= nearest->get_stats().max_hunger * 0.3f);

					if (can_reproduce && enough_hunger && prob_dist(rng) <= REPRODUCE_PROB)
					{
						static std::mt19937 rng(std::random_device{}());
						std::uniform_real_distribution<float> variation(-0.1f, 0.1f);
						Stats child_stats;

						// --- Size con mutación ---
						child_stats.size = (og->get_stats().size + nearest->get_stats().size) / 2.0f;
						child_stats.size += child_stats.size * variation(rng);
						child_stats.size = std::clamp(child_stats.size, 0.5f, 5.0f);

						// --- Derivados de size ---
						child_stats.weight = std::pow(child_stats.size, 3.0f);
						child_stats.hp = 100.0f * child_stats.size;
						child_stats.max_hunger = 200.0f * child_stats.size;
						child_stats.hunger = child_stats.max_hunger * 0.5f;
						child_stats.nu = child_stats.weight * 50
							;
						child_stats.stamina = 100.0f * child_stats.size;

						// --- Speed con rango dependiente de size ---
						float speed_min = 3.0f + 2.0f * child_stats.size;
						float speed_max = 8.0f + 4.0f * child_stats.size;
						child_stats.speed = (og->get_stats().speed + nearest->get_stats().speed) / 2.0f;
						child_stats.speed += child_stats.speed * variation(rng);
						child_stats.speed = std::clamp(child_stats.speed, speed_min, speed_max);

						// --- Vision con rango dependiente de size ---
						float vision_base = 5.0f + 2.0f * child_stats.size;
						float vision_min = vision_base * 0.8f;
						float vision_max = vision_base * 1.2f;
						child_stats.vision = (og->get_stats().vision + nearest->get_stats().vision) / 2.0f;
						child_stats.vision += child_stats.vision * variation(rng);
						child_stats.vision = std::clamp(child_stats.vision, vision_min, vision_max);

						// --- Categoría heredada ---
						child_stats.category = og->get_stats().category;

						// --- Color con mezcla y mutación ---
						float t = 0.5f;
						auto mix_channel = [&](sf::Uint8 a, sf::Uint8 b)
							{
								float base = a * (1 - t) + b * t;
								float mutated = base + base * variation(rng);
								return static_cast<sf::Uint8>(std::clamp(mutated, 0.0f, 255.0f));
							};
						child_stats.color = sf::Color(
							mix_channel(og->get_stats().color.r, nearest->get_stats().color.r),
							mix_channel(og->get_stats().color.g, nearest->get_stats().color.g),
							mix_channel(og->get_stats().color.b, nearest->get_stats().color.b)
						);

						// --- Crear hijo ---
						std::shared_ptr<Organism> child = EntityFactory<Organism>::create("Organism", child_stats);
						sf::Vector2f child_pos = pos;
						child_pos.x += static_cast<float>((std::rand() % 3) - 1);
						child_pos.y += static_cast<float>((std::rand() % 3) - 1);
						child->get_transform().set_position(child_pos);
						Scene::instance().add_entity(child);
						EventManager::publish(Event(EventType::ORGANISM_BORN, EntityEvent(child)), child);

						// --- Coste energético de reproducción ---
						og->get_stats().hunger = std::max(0.0f, (og->get_stats().hunger - (og->get_stats().max_hunger * 0.3f)));
						nearest->get_stats().hunger = std::max(0.0f, (nearest->get_stats().hunger - (nearest->get_stats().max_hunger * 0.3f)));

						cooldown_ticks[og.get()] = COOLDOWN_TICKS;
						cooldown_ticks[nearest.get()] = COOLDOWN_TICKS;

						moving = false;
						fixed_entity.reset();

						return BTStatus::SUCCESS;
					}
					else
					{
						return BTStatus::RUNNING;
					}
				}
				else
				{
					fixed_entity = nearest;
					goal = fixed_entity->get_transform().get_position();
					sf::Vector2f dir = goal - pos;
					float angle_rad = std::atan2(dir.y, dir.x);
					float angle_deg = angle_rad * 180.0f / Constants::pi_val;
					og->get_transform().set_rotation(angle_deg);
					moving = true;
					return BTStatus::RUNNING;
				}
			}

			if (!moving && !fixed_entity)
			{
				static std::mt19937 rng_walk(std::random_device{}());
				std::uniform_real_distribution<float> angle_deg_dist(0.0f, 360.0f);
				std::uniform_real_distribution<float> radius_dist(0.0f, 10.0f);

				float angle_deg = angle_deg_dist(rng_walk);
				const float angle_rad = angle_deg * Constants::pi_val / 180.0f;
				const float radius = radius_dist(rng_walk);

				sf::Vector2f offset(std::cos(angle_rad) * radius, std::sin(angle_rad) * radius);
				goal = pos + offset;

				auto terrain_entity = Scene::instance().get_entity("Terrain");
				if (auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity))
				{
					float map_w = static_cast<float>(terrain->get_width());
					float map_h = static_cast<float>(terrain->get_height());

					if (goal.x < 0.f || goal.x > map_w - 1.0f ||
						goal.y < 0.f || goal.y > map_h - 1.0f)
					{
						offset = -offset;
						goal = pos + offset;
						angle_deg = std::atan2(offset.y, offset.x * 180.0f / Constants::pi_val);
					}

					goal.x = std::max(0.f, std::min(goal.x, map_w - 1.0f));
					goal.y = std::max(0.f, std::min(goal.y, map_h - 1.0f));
				}

				og->get_transform().set_rotation(angle_deg);
				moving = true;
				return BTStatus::RUNNING;
			}

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
	time += Time::get_delta() * Time::get_simulation_speed();

	if (auto organism = std::dynamic_pointer_cast<Organism>(owner.lock()))
	{
		Stats& s = organism->get_stats();
		const float k_basal = 1.5f;
		float drain = (k_basal * s.size) * Time::get_delta() * Time::get_simulation_speed();

		s.hunger -= drain;

		if (s.hunger <= 0.0f)
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

FoodSpawner::FoodSpawner(std::weak_ptr<Entity> _owner) : Component(_owner) 
{
	timer = static_cast<float>(std::rand() % (int)spawn_rate);
}

Tree::Tree(std::string _name) : Entity(_name) {}

void Tree::init()
{
    add_component(std::make_shared<SpriteRenderer>(shared_from_this(), "tree"));

    add_component(std::make_shared<FoodSpawner>(shared_from_this()));
}

void FoodSpawner::update()
{
	time += Time::get_delta() * Time::get_simulation_speed();

    if (time >= spawn_rate)
    {
        time = 0.0f;

        auto owner_ptr = owner.lock();
        if (!owner_ptr) return;

        std::shared_ptr<Food> food = EntityFactory<Food>::create("fruit");
        
        sf::Vector2f tree_pos = owner_ptr->get_transform().get_position();
 
        float angle = (std::rand() % 360) * Constants::pi_val / 180.0f;
        float dist = (std::rand() % 100) / 100.0f * spawn_radius; // 0 to spawn_radius
        
        sf::Vector2f offset(std::cos(angle) * dist, std::sin(angle) * dist);
        sf::Vector2f spawn_pos_meters = (tree_pos + offset);

        auto terrain_entity = Scene::instance().get_entity("Terrain");
        if (auto terrain = std::dynamic_pointer_cast<EntityTerrain>(terrain_entity))
        {
            sf::Vector2f offset_px = offset;
            sf::Vector2f final_pos_px = tree_pos + offset_px;

            int gx = static_cast<int>(final_pos_px.x);
            int gy = static_cast<int>(final_pos_px.y);

            if (terrain->plantable((uint16_t)gx, (uint16_t)gy))
            {
                food->get_transform().set_position(final_pos_px);
                Scene::instance().add_entity(food);
            }
        }
    }
}
