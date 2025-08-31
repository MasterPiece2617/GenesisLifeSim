#include <utils.hpp>

sf::Clock Time::clock;
sf::Time Time::delta_time;

void Time::update()
{
	delta_time = clock.restart();
}

float Time::get_delta()
{
	return delta_time.asSeconds();
}