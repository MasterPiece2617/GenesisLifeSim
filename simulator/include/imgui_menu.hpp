#pragma once

#include<vector>
#include<memory>

#include <imgui-SFML.h>
#include <imgui.h>

#include <entity_terrain.hpp>
#include <scene.hpp>
#include <texture.hpp>

class ImGuiMenu {
protected:
public:
    static void show_select_map_window(bool& map_loaded, std::string& selected_map, const std::vector<std::string>& map_files, 
                                        std::shared_ptr<EntityTerrain>& terrain, Scene scene, std::shared_ptr<Atlas> texture_atlas);
};

class ImPlotMenu {
protected:
public:
    // static void show_plot_window();
};