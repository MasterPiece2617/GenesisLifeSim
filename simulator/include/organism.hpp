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

struct Stats
{
	float hunger = 100;
	int vision = 5;
	sf::Color color = sf::Color::Red;
};

class Organism : public Entity
{
protected:
	Stats stats;
	bool is_alive = true;
	OrganismConfig organism_config;
public:
	Organism(std::string _name, const OrganismConfig& _organism_config);
	void init() override;
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
	float speed = 5;
public:
	Behaviour(std::weak_ptr<Entity> _owner, const OrganismConfig& _organism_config);
	void start() override;
	void update() override;
	~Behaviour() = default;
};

class Food : public Entity
{
protected:
	int nu = 25;

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
	float time = 0;
public:
	FoodSpawner(std::weak_ptr<Entity> _owner);
	void update() override;
};