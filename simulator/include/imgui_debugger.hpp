#pragma once

#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics.hpp>

#include <entity_terrain.hpp>
#include <scene.hpp>

class Debugger 
{
protected:
public:
    //auto 
    static void imgui_terrain(std::shared_ptr<EntityTerrain> t, std::shared_ptr<sf::RenderWindow> window);
    static void imgui_scene(Scene scene); 
};