#pragma once

#include <SFML/Graphics.hpp>
#include "organism.hpp"

struct OrganismConfig
{
	float hunger = 100;
	float vision_radius = 5;
	float move_speed = 5.0f;
	sf::Color color = sf::Color::White;
};