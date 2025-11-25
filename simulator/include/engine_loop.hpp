#pragma once

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>

#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>
#include <implot.h>

#include <config.hpp>
#include <entity_terrain.hpp>
#include <entity_test.hpp>
#include <event_manager.hpp>
#include <renderer.hpp>
#include <scene.hpp>
#include <texture.hpp>
#include <unordered_set>
#include <utils.hpp>
#include <organism_config.hpp>
#include <imgui_debugger.hpp>
#include <imgui_menu.hpp>
#include <population_stats.hpp>

class Engine {
protected:
  std::shared_ptr<sf::RenderWindow> window;
  // To load terrain
  std::shared_ptr<Atlas> texture_atlas;
  std::shared_ptr<EntityTerrain> terrain;
  static std::string selected_map;
  bool map_loaded = false;
  // Main scene
  Scene scene;
  // std::vector<std::vector<std::shared_ptr<SpriteRenderer>>> render_queue =
  // std::vector<std::vector<std::shared_ptr<SpriteRenderer>>>(256,
  // std::vector<std::shared_ptr<SpriteRenderer>>());

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
  static void reset_simulation(std::shared_ptr<EntityTerrain>& terrain, bool& map_loaded, std::string& selected_map);
};
