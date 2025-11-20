#include <imgui_menu.hpp>

void ImGuiMenu::show_select_map_window(bool& map_loaded, std::string& selected_map, const std::vector<std::string>& map_files, 
                                        std::shared_ptr<EntityTerrain>& terrain, std::shared_ptr<Atlas>& texture_atlas)
{
    ImGui::Begin("Selccionar mapa");
  
    ImGui::Text("Mapas disponibles:");
    ImGui::Separator();

    for (const auto& map_file : map_files)
    {
        if (ImGui::Selectable(map_file.c_str(), selected_map == map_file))
        {
            selected_map = map_file;
        }
    }

    ImGui::Separator();
    ImGui::Text("Mapa seleccionado: %s", selected_map.c_str());

    if (ImGui::Button("Cargar mapa") && !selected_map.empty())
    {
        texture_atlas = std::make_shared<Atlas>();
        terrain = EntityFactory<EntityTerrain>::create(selected_map, texture_atlas);
        Scene::instance().add_entity(terrain);
        map_loaded = true; // To close the map selection window
        ImGui::CloseCurrentPopup();
    } 
    ImGui::End();
}