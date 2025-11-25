#pragma once

#include <SFML/Graphics.hpp>

enum class OrganismCategory
{
	HERBIVORE,
	CARNIVORE
};

struct OrganismConfig
{
	float hp = 100;
	float max_hp = 100;
	float hunger = 200;
	float max_hunger = 200;
	float stamina = 100;
	float max_stamina = 100;
	float size = 1;
	float weight = 1;
	float vision_radius = 5;
	float nu = 1;
	float move_speed = 5.0f;
	float swim_speed = 5.0f;
	OrganismCategory category = OrganismCategory::HERBIVORE;
	sf::Color color = sf::Color::White;
};