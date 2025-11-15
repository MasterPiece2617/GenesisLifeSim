#pragma once

#include <iostream>
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>

#include <config.hpp>
#include <entity_test.hpp>
#include <event_manager.hpp>
#include <renderer.hpp>
#include <scene.hpp>
#include <texture.hpp>
#include <unordered_set>
#include <utils.hpp>
#include <organism_config.hpp>

class Engine
{
protected:

	std::shared_ptr<sf::RenderWindow> window;
	Scene scene;
	std::shared_ptr<Entity> selected_entity;
	OrganismConfig organism_config;

	bool caract_load = false;

	//std::vector<std::vector<std::shared_ptr<SpriteRenderer>>> render_queue = std::vector<std::vector<std::shared_ptr<SpriteRenderer>>>(256, std::vector<std::shared_ptr<SpriteRenderer>>());
	int frame_count = 0;
	double frame_times = 0.0f;

	void update();
	void render();

public:

	// Constructor and Destructor
	Engine();
	~Engine();

	// Public methods
	void run();
};