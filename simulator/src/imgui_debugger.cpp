#include <imgui_debugger.hpp>

void Debugger::imgui_terrain(std::shared_ptr<EntityTerrain> terrain, std::shared_ptr<sf::RenderWindow> window)
{
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
    } else {
      ImGui::Separator();
      ImGui::Text("Fuera de los limites del mapa");
    }

    ImGui::End();
    }
}

void Debugger::imgui_scene(Scene scene)
{
    ImGui::Begin("Info");
    ImGui::Text("Entities: %d", scene.get_entities().size());
    ImGui::Text("FPS: %.2f", 1.0f / Time::get_delta());
    ImGui::End();
}