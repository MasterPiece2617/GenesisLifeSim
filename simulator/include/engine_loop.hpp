#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>

#include <entity_test.hpp>
#include <event_manager.hpp>
#include <renderer.hpp>
#include <scene.hpp>

class Engine
{
protected:

	std::shared_ptr<sf::RenderWindow> window;
	Scene scene;
	std::vector<std::vector<std::shared_ptr<Renderer>>> render_queue = std::vector<std::vector<std::shared_ptr<Renderer>>>(256, std::vector<std::shared_ptr<Renderer>>());

	void update();
	void render();

public:

	// Constructor and Destructor
	Engine();
	~Engine();

	// Public methods
	void run();
};