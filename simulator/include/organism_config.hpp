#pragma once

#include <SFML/Graphics.hpp>

struct OrganismConfig
{
	int vision_radius = 5;

	float move_speed = 5.0f;

	sf::Color color = sf::Color::White;
};