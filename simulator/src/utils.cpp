#include <utils.hpp>

float Time::simulation_speed = 1.0f;
sf::Clock Time::clock;
sf::Time Time::delta_time;

void Time::update()
{
	delta_time = clock.restart();
}

void Time::set_simulation_speed(float speed)
{
	simulation_speed = speed;
}

float Time::get_simulation_speed()
{
	return simulation_speed;
}

float Time::get_delta()
{
	return delta_time.asSeconds();
}