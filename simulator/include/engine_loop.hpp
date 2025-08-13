#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>

class Engine
{
private:

	// Private variables
	std::shared_ptr<sf::RenderWindow> window;

	// Private methods
	void update();
	void render();

public:

	// Constructor and Destructor
	Engine();
	~Engine();

	// Public methods
	void run();
};