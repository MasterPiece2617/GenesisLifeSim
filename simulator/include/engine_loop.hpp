#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>

#include <entity_test.hpp>
#include <event_manager.hpp>
#include <scene.hpp>

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