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

void ImGuiMenu::show_organism_config_window(OrganismConfig& organism_config, sf::RenderWindow* window)
{
    ImGui::Begin("Seleccione las caracteristicas:");
    ImGui::Text("Seleccione las caracteristicas que desea cargar en la simulacion.");
    ImGui::Separator();
    ImGui::Text("Stats del Organismo:");
    ImGui::SliderInt("Vision", &organism_config.vision_radius, 1, 20);
    ImGui::Separator();
    ImGui::Text("Stats de Comportamiento:");
    ImGui::SliderFloat("Velocidad (Speed)", &organism_config.move_speed, 1.0f, 15.0f);
    {
        sf::Color sc = organism_config.color;
        float ccol[4] = { sc.r / 255.0f, sc.g / 255.0f, sc.b / 255.0f, sc.a / 255.0f };
        if (ImGui::ColorEdit4("Color del Organismo", ccol))
        {
            organism_config.color = sf::Color(
                static_cast<sf::Uint8>(ccol[0] * 255.0f),
                static_cast<sf::Uint8>(ccol[1] * 255.0f),
                static_cast<sf::Uint8>(ccol[2] * 255.0f),
                static_cast<sf::Uint8>(ccol[3] * 255.0f)
            );
        }
    }
    ImGui::Separator();

    ImGui::Text("cantidad de organismos a generar: 4 (fijo por ahora)");
    ImGui::SliderInt("Cantidad de organismos", &Scene::instance().num_organisms, 1, 100);
    if (ImGui::Button("Cargar Caracteristicas"))
    {
        Scene::instance().load(organism_config);
        window->setView(Scene::instance().get_main_camera()->get_view());
    }
    ImGui::End();
}

static bool show_organism_population = false;
static bool show_organism_config = false;

void ImGuiMenu::show_menu(OrganismConfig& organism_config, sf::RenderWindow* window)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Plots"))
        {
            ImGui::MenuItem("Organism Population", NULL, &show_organism_population);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Organism")) 
        {
            ImGui::MenuItem("Organism Config", NULL, &show_organism_config);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (show_organism_population)
    {
        ImPlotMenu::organism_population_plot();
    }

    if (show_organism_config)
    {
        ImGuiMenu::show_organism_config_window(organism_config, window);
    }
}


// Graficas de Implot
void ImPlotMenu::organism_population_plot()
{
    ImGui::Begin("Organism Population Over Time Plot");

    // Use vectors to store data dynamically
    static std::vector<float> time_data;
    static std::vector<float> population_data;
    
    // Get current time and organism count
    float current_time = ImGui::GetTime();
    int organism_count = 0;
    for (const auto& entity : Scene::instance().get_entities())
    {
        if (std::dynamic_pointer_cast<Organism>(entity))
        {
            organism_count++;
        }
    }

    // Add new data point
    time_data.push_back(current_time);
    population_data.push_back(static_cast<float>(organism_count));

    // Remove data older than 'history'
    float history = 60.0f;
    auto first_to_keep = std::lower_bound(time_data.begin(), time_data.end(), current_time - history);
    if (first_to_keep != time_data.begin()) {
        auto num_to_erase = std::distance(time_data.begin(), first_to_keep);
        time_data.erase(time_data.begin(), first_to_keep);
        population_data.erase(population_data.begin(), population_data.begin() + num_to_erase);
    }

    ImPlot::SetNextAxisLimits(ImAxis_X1, current_time - history, current_time, ImGuiCond_Always);
    ImPlot::SetNextAxisToFit(ImAxis_Y1);

    if (ImPlot::BeginPlot("Organism Population Over Time", "Time", "Population"))
    {
        // Plot the data
        if (!time_data.empty())
        {
            ImPlot::PlotLine("Population", time_data.data(), population_data.data(), time_data.size());
        }

        ImPlot::EndPlot();
    }
    ImGui::End();
}