#include <imgui_debugger.hpp>

void Debugger::imgui_terrain(std::shared_ptr<EntityTerrain> terrain, std::shared_ptr<sf::RenderWindow> window)
{
    sf::Vector2i mouse_pos_win = sf::Mouse::getPosition(*window);

    // Convierte la posicion del mouse a coordenadas del mundo (ajustando por la
    // camara)
    sf::Vector2f mouse_pos_world =
        window->mapPixelToCoords(mouse_pos_win);

    uint16_t cell_x =
        static_cast<uint16_t>(mouse_pos_world.x / Constants::px_mt);
    uint16_t cell_y =
        static_cast<uint16_t>(mouse_pos_world.y / Constants::px_mt);

    ImGui::Begin("Info de Celda");
    ImGui::Text("Posicion del mouse (mundo): (%.1f, %.1f)", mouse_pos_world.x,
                mouse_pos_world.y);

    if (cell_x < terrain->get_width() && cell_y < terrain->get_height()) 
    {
      CellData cell_data = terrain->get_cell(cell_x, cell_y);

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

void Debugger::imgui_scene(float& fps_display, sf::Vector2f& mouse_world_pos)
{
    ImGui::Begin("Info");
    ImGui::Text("Entities: %d", Scene::instance().get_entities().size());
    ImGui::Text("FPS: %.2f", fps_display);
    ImGui::Text("Zoom: %.2f", Scene::instance().get_main_camera()->get_zoom());
    ImGui::Text("Mouse x: %f y: %f", mouse_world_pos.x / Constants::px_mt, mouse_world_pos.y / Constants::px_mt);
    ImGui::End();
}

void Debugger::show_selected_entity(std::shared_ptr<Organism>& selected_entity)
{
    if (!selected_entity)
    {
        return;
    }
    ImGui::Begin("Selected Entity");
    ImGui::Text("Name: %s", selected_entity->get_name().c_str());
    ImGui::Separator();
    ImGui::Text("Position: %.2f, %.2f", selected_entity->get_transform().get_position().x, selected_entity->get_transform().get_position().y);
    ImGui::Text("type organism: %s", selected_entity->get_stats().category == OrganismCategory::CARNIVORE ? "Carnivore" : "Herbivore");
    ImGui::End();
}