#pragma once

#include<vector>
#include<memory>

#include <imgui-SFML.h>
#include <imgui.h>
#include <implot.h>

#include <entity_terrain.hpp>
#include <scene.hpp>
#include <texture.hpp>
#include <organism_config.hpp>
#include <population_stats.hpp>

class Engine; // Forward declaration

class ImGuiMenu 
{
protected:
public:

    static void show_menu(OrganismConfig& organism_config, sf::RenderWindow* window, 
                          std::shared_ptr<EntityTerrain>& terrain, bool& map_loaded, std::string& selected_map);
    static void show_select_map_window(bool& map_loaded, std::string& selected_map, const std::vector<std::string>& map_files, 
                                        std::shared_ptr<EntityTerrain>& terrain, std::shared_ptr<Atlas>& texture_atlas);

    static void show_organism_config_window(OrganismConfig& organism_config, sf::RenderWindow* window);
    static void call_reset_simulation(std::shared_ptr<EntityTerrain>& terrain, bool& map_loaded, std::string& selected_map);
};

class ImPlotMenu : public ImGuiMenu
{
protected:
public:
    static void organism_population_plot();
    static void carnivore_herbivore_plot();
    static void organism_stats_plot();
};