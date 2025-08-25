#pragma once 

#include <SFML/System.hpp>

class Time
{
protected:

	static sf::Time delta_time;
	static sf::Clock clock;

public:

	static void update();

	static float get_delta();
};