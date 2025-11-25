#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <random>
#include <unordered_set>
#include <vector>
#include <memory>
#include <numbers>

#include <SFML/System.hpp>

#include <behaviour_tree.hpp>
#include <component.hpp>
#include <entity.hpp>
#include <renderer.hpp>
#include <scene.hpp>
#include <utils.hpp>
#include <organism_config.hpp>
#include <entity_terrain.hpp>
#include <pathfinder.hpp>

struct Stats
{
	float hp = 100;
	float max_hp = 100;
	float hunger = 100;
	float max_hunger = 100;
	float stamina = 100;
	float max_stamina = 100;
	float size = 1;
	float weight = 1;
	float vision = 5;
	float nu = 100;
	float speed = 5.0f;
	float swim = 5.0f;
	OrganismCategory category = OrganismCategory::HERBIVORE;
	sf::Color color = sf::Color::Red;
};

class Organism : public Entity
{
protected:
	Stats stats;
	bool is_alive = true;
	bool is_resting = false;
	std::string state = "Idle";

public:
	Organism(std::string _name, const Stats _stats);
	Organism(std::string _name, const OrganismConfig& _organism_config);
	bool get_is_resting();
	void set_is_resting(bool rest);
	void init() override;
	std::string get_state();
	void set_state(std::string new_state);
	Stats& get_stats();
};

class Behaviour : public Component
{
protected:
	sf::Vector2f goal;
	std::shared_ptr<Entity> fixed_entity;
	BehaviourTree bt;
	float time = 0;
	bool moving = false;
	// Para el pathfinder
	PathFinder pathfinder;
	std::vector<sf::Vector2f> path;
    int path_index = path.size();
public:
	Behaviour(std::weak_ptr<Entity> _owner);
	void start() override;
	void update() override;
	~Behaviour() = default;
};

class Food : public Entity
{
protected:
	int nu = 50;

public:
	Food(std::string _name);
	void init() override;
	int get_nu();
};

class FoodGenerator : public Entity
{
public:
	FoodGenerator(std::string _name);
	void init() override;
};

class FoodSpawner : public Component
{
protected:
	float timer = 0.0f;
	float time = 0.0f;
    float spawn_rate = 15.0f; // Segundos para generar una fruta
    float spawn_radius = 2.0f; // Radio en metros donde cae la fruta
public:
	FoodSpawner(std::weak_ptr<Entity> _owner);
	void update() override;
};

// La Entidad Árbol
class Tree : public Entity
{
public:
    Tree(std::string _name);
    void init() override;
};
