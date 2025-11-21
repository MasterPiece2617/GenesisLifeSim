#include <imgui_debugger.hpp>

void Debugger::imgui_terrain(std::shared_ptr<EntityTerrain> terrain, std::shared_ptr<sf::RenderWindow> window)
{
    sf::Vector2i mouse_pos_win = sf::Mouse::getPosition(*window);

    // Convierte la posicion del mouse a coordenadas del mundo (ajustando por la
    // camara)
    sf::Vector2f mouse_pos_world =
        window->mapPixelToCoords(mouse_pos_win);

    // Calcula las coordenadas de la celda
    uint16_t cell_x =
        static_cast<uint16_t>(mouse_pos_world.x / Constants::px_mt);
    uint16_t cell_y =
        static_cast<uint16_t>(mouse_pos_world.y / Constants::px_mt);

    ImGui::Begin("Info de Celda");
    ImGui::Text("Posicion del mouse (mundo): (%.1f, %.1f)", mouse_pos_world.x,
                mouse_pos_world.y);

    // Verifica si la celda esta dentro de los limites del mapa
    if (cell_x < terrain->get_width() && cell_y < terrain->get_height()) 
    {
      // Obtiene los datos de la celda
      CellData cell_data = terrain->get_cell(cell_x, cell_y);

      // Muestra los datos de la celda
      ImGui::Separator();
      ImGui::Text("Coordenadas de la celda: (%d, %d)", cell_x, cell_y);
      ImGui::Text("Tipo de terreno: %d", cell_data.terrain_type);
      ImGui::Text("ID de textura: %d", cell_data.texture_id);
      ImGui::Text("caminable: %s", cell_data.is_walkable ? "Si" : "No");
      ImGui::Text("Esfuerzo: %d", cell_data.effort);
    } else {
      ImGui::Separator();
      ImGui::Text("Fuera de los limites del mapa");
    }

    ImGui::End();
}

void Debugger::imgui_scene(float& fps_display, sf::Vector2f& mouse_world_pos, std::shared_ptr<Entity>& selected_entity)
{
    ImGui::Begin("Info");
    ImGui::Text("Entities: %d", Scene::instance().get_entities().size());
    ImGui::Text("FPS: %.2f", fps_display);
    ImGui::Text("Zoom: %.2f", Scene::instance().get_main_camera()->get_zoom());
    ImGui::Text("Mouse x: %f y: %f", mouse_world_pos.x, mouse_world_pos.y);
    if (selected_entity)
    {
        ImGui::Text("%s", selected_entity->get_name().c_str());
    }
    ImGui::End();
}