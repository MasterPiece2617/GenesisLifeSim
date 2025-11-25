#pragma once

#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics.hpp>

#include <entity_terrain.hpp>
#include <scene.hpp>
#include <utils.hpp>

class Debugger 
{
protected:
public:
    //auto 
    static void imgui_terrain(std::shared_ptr<EntityTerrain> t, std::shared_ptr<sf::RenderWindow> window);
    static void imgui_scene(float& fps_display, sf::Vector2f& mouse_world_pos); 
    static void show_selected_entity(std::shared_ptr<Organism>& selected_entity);

};