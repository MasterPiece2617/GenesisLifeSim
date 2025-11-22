#pragma once

#include <SFML/Graphics.hpp>

enum class OrganismCategory
{
	HERBIVORE,
	CARNIVORE
};

struct OrganismConfig
{
	float hunger = 100;
	float vision_radius = 5;
	float move_speed = 5.0f;
	OrganismCategory category = OrganismCategory::HERBIVORE;
	sf::Color color = sf::Color::White;
};