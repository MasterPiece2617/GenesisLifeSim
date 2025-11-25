#include <imgui_menu.hpp>
#include <engine_loop.hpp>

bool is_carnivore = false;
bool show_map_creator_window = false;
int width = 200;
int height = 200;

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
        show_map_creator_window = false;
        ImGui::CloseCurrentPopup();
    } 

    if (ImGui::Button("Crear mapa personalizado"))
    {
        show_map_creator_window = true;
    }

    if (show_map_creator_window)
    {
        show_map_creator();
    }

    ImGui::End();
}

void ImGuiMenu::show_map_creator()
{
    ImGui::Begin("Creador de Mundos");

    static MapGenerator::Config config;
    static char filename_buffer[128] = "new_map.zadat";

    ImGui::Text("Configuracion del Terreno");
    ImGui::Separator();

    // Inputs
    ImGui::InputInt("Semilla (Seed)", &config.seed);
    ImGui::SameLine();
    if (ImGui::Button("Random")) {
        config.seed = rand();
    }

    ImGui::SliderFloat("Frecuencia (Zoom)", &config.frequency, 0.001f, 0.1f, "%.4f");
    ImGui::SliderFloat("Detalle (Ruido)", &config.detail_freq, 0.01f, 0.2f);
    ImGui::SliderInt("Octavas", &config.octaves, 1, 8);

    ImGui::Separator();
    ImGui::Text("Dimensiones");
    ImGui::InputInt("Ancho", &width, 10, 100);
    ImGui::InputInt("Alto", &height, 10, 100);

    config.width = static_cast<uint16_t>(width);
    config.height = static_cast<uint16_t>(height);

    ImGui::Separator();
    ImGui::InputText("Nombre Archivo", filename_buffer, sizeof(filename_buffer));

    if (ImGui::Button("Cerrar", ImVec2(120, 0))) 
    { 
        show_map_creator_window = false;
    }

    if (ImGui::Button("Generar y guardar mapa", ImVec2(-1, 25)))
    {   
        config.filename = std::string(filename_buffer);

        // Validar extensión
        if (config.filename.find(".zadat") == std::string::npos) {
            config.filename += ".zadat";
        }

        // Llamar al generador
        MapGenerator::generate_map(config);
        
        ImGui::OpenPopup("Mapa Guardado");
    }

    // Modal de confirmación
    if (ImGui::BeginPopupModal("Mapa Guardado", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("El mapa se ha guardado en resources/maps/");
        ImGui::Text("%s", config.filename.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            ImGui::CloseCurrentPopup(); 
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void ImGuiMenu::show_organism_config_window(OrganismConfig& organism_config, sf::RenderWindow* window, bool& placement_mode)
{
    ImGui::Begin("Seleccione las caracteristicas:");
    ImGui::Text("Stats del Organismo:");
    ImGui::Separator();

    if (ImGui::SliderFloat("Tamaño (Size)", &organism_config.size, 0.5f, 5.0f))
    {
        // show stats and recalculus
        organism_config.weight = std::pow(organism_config.size, 3.0f);
        organism_config.hp = 100.0f * organism_config.size;
        organism_config.max_hunger = 200.0f * organism_config.size;
        organism_config.nu = organism_config.weight * 50;
        organism_config.stamina = 100.0f * organism_config.size;
		organism_config.max_hp = organism_config.hp;
		organism_config.hunger = organism_config.max_hunger * 0.69f;
		organism_config.max_stamina = organism_config.stamina;

        float speed_min = 3.0f + 2.0f * organism_config.size;
        float speed_max = 8.0f + 4.0f * organism_config.size;
        organism_config.move_speed = std::clamp(organism_config.move_speed, speed_min, speed_max);

        float vision_base = 5.0f + 2.0f * organism_config.size;
        float vision_min = vision_base * 0.8f;
        float vision_max = vision_base * 1.2f;
        organism_config.vision_radius = std::clamp(organism_config.vision_radius, vision_min, vision_max);
    }

    float speed_min = 3.0f + 2.0f * organism_config.size;
    float speed_max = 8.0f + 4.0f * organism_config.size;
    ImGui::SliderFloat("Velocidad (Speed)", &organism_config.move_speed, speed_min, speed_max);

    float vision_base = 5.0f + 2.0f * organism_config.size;
    float vision_min = vision_base * 0.8f;
    float vision_max = vision_base * 1.2f;
    ImGui::SliderFloat("Vision", &organism_config.vision_radius, vision_min, vision_max);

    speed_min = 4.0f - 1.0f * organism_config.size;
    speed_max = 25.0f - 2.0f * organism_config.size;
    ImGui::SliderFloat("Velocidad de Nado", &organism_config.swim_speed, speed_min, speed_max);

    ImGui::Separator();
    ImGui::Text("HP: %.1f", organism_config.hp);
    ImGui::Text("Hunger: %.1f", organism_config.hunger);
    ImGui::Text("Stamina: %.1f", organism_config.stamina);
    ImGui::Text("Peso (Weight): %.1f", organism_config.weight);
    ImGui::Text("Nutrición (nu): %.1f", organism_config.nu);

    // Color
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

    ImGui::Separator();
    ImGui::Checkbox("Es Carnivoro", (bool*)&is_carnivore);
    organism_config.category = is_carnivore ? OrganismCategory::CARNIVORE : OrganismCategory::HERBIVORE;

    ImGui::Text("cantidad de organismos a generar");
    ImGui::SliderInt("Cantidad de organismos", &Scene::instance().num_organisms, 1, 20);

    int styles_pushed = 0;

    if (placement_mode)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));        
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.9f, 0.3f, 1.0f)); 
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));  
        styles_pushed = 3;
    }

    const char* label = placement_mode ? "Modo Colocacion: ACTIVO [Click para Apagar]###ToggleSpawn" 
                                       : "Modo Colocacion: INACTIVO [Click para Encender]###ToggleSpawn";

    if (ImGui::Button(label, ImVec2(-1, 40)))
    {
        placement_mode = !placement_mode;
    }

    if (styles_pushed > 0) 
    {
        ImGui::PopStyleColor(styles_pushed);
    }

    if (placement_mode) 
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), ">> MODO ACTIVO <<");
        ImGui::TextWrapped("- Click Izquierdo en el mapa: Crear Organismo");
        ImGui::TextWrapped("- Click Derecho: Cancelar / Salir");
    }

    ImGui::End();
}

void ImGuiMenu::call_reset_simulation(std::shared_ptr<EntityTerrain>& terrain, bool& map_loaded, std::string& selected_map, bool& centered)
{
    Engine::reset_simulation(terrain, map_loaded, selected_map, centered);
}

static bool show_organism_population = false;
static bool show_organism_config = true; // Provisional for tests, then we decided what will do with this
static bool show_carnivore_herbivore = false;
static bool show_organism_stats = false;
static bool reset_simulation_requested = false;


void ImGuiMenu::show_menu(OrganismConfig& organism_config, sf::RenderWindow* window, 
                          std::shared_ptr<EntityTerrain>& terrain, bool& map_loaded, std::string& selected_map, bool& centered, bool& placement_mode)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Graficos"))
        {
            ImGui::MenuItem("Poblacion", NULL, &show_organism_population);
            ImGui::MenuItem("Estadisticas de organismos", NULL, &show_organism_stats);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Organismo")) 
        {
            ImGui::MenuItem("Configurar Organismo", NULL, &show_organism_config);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Simulacion"))
        {
            if (ImGui::MenuItem("Reiniciar Simulacion"))
            {
                ImGuiMenu::call_reset_simulation(terrain, map_loaded, selected_map, centered);
                reset_simulation_requested = true;
            }
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
        ImGuiMenu::show_organism_config_window(organism_config, window, placement_mode);
    }

    if (show_organism_stats)
    {
        ImPlotMenu::organism_stats_plot();
    }
}

// helper function to resample a curve to N points
struct Point { float x, y; };
std::vector<Point> resample_curve(const std::vector<Point>& data, size_t N) 
{
    if (data.size() < 2) return data;

    // Longitud acumulada
    std::vector<float> s(data.size(), 0.0f);
    for (size_t i = 1; i < data.size(); ++i) {
        float dx = data[i].x - data[i-1].x;
        float dy = data[i].y - data[i-1].y;
        s[i] = s[i-1] + std::sqrt(dx*dx + dy*dy);
    }
    float total_length = s.back();

    std::vector<Point> out;
    out.reserve(N);

    size_t k = 0;
    for (size_t i = 0; i < N; ++i) {
        float target = (total_length * i) / (N - 1);

        while (k+1 < data.size() && s[k+1] < target) ++k;

        if (k+1 >= data.size()) {
            out.push_back(data.back());
            continue;
        }

        float t = (target - s[k]) / (s[k+1] - s[k]);
        Point p;
        p.x = data[k].x + t * (data[k+1].x - data[k].x);
        p.y = data[k].y + t * (data[k+1].y - data[k].y);
        out.push_back(p);
    }
    return out;
}

// ImPlot graphs
void ImPlotMenu::organism_population_plot()
{
    static bool real_time_view = true;
    static bool show_all_history = false;
    static bool last_minute_static = false;
    static bool last_5_minutes = false;

    ImGui::Begin("Poblacion de Organismos");

    if (ImGui::Checkbox("Tiempo Real", &real_time_view)) 
    {
        if(real_time_view) 
        { 
            show_all_history=false; 
            last_minute_static=false; 
            last_5_minutes=false; 
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Toda la Historia", &show_all_history)) 
    {
        if(show_all_history) 
        { 
            real_time_view=false; 
            last_minute_static=false; 
            last_5_minutes=false; 
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Ultimo Minuto", &last_minute_static)) 
    {
        if(last_minute_static) 
        { 
            real_time_view=false; 
            show_all_history=false; 
            last_5_minutes=false; 
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Ultimos 5 Min", &last_5_minutes)) 
    {
        if(last_5_minutes) 
        { 
            real_time_view=false; 
            show_all_history=false; 
            last_minute_static=false; 
        }
    }

    ImGui::Separator();

    const auto& raw_time = PopulationStats::get_time_history();
    const auto& raw_tot  = PopulationStats::get_total_organisms_history();
    const auto& raw_carn = PopulationStats::get_carnivores_history();
    const auto& raw_herb = PopulationStats::get_herbivores_history();

    const auto* p_time = raw_time.data();
    const auto* p_tot  = raw_tot.data();
    const auto* p_carn = raw_carn.data();
    const auto* p_herb = raw_herb.data();
    int count = raw_time.size();

    std::shared_ptr<std::vector<float>> view_t;
    std::shared_ptr<std::vector<float>> view_tot, view_c, view_h;

    if (!real_time_view && !raw_time.empty())
    {
        float current_t = PopulationStats::get_time(); // raw_time.back()
        float window = 0.0f;

        if (last_minute_static) window = 60.0f;
        if (last_5_minutes) window = 300.0f;
        if (show_all_history) window = current_t;

        // Buscar índice de inicio
        auto it = std::lower_bound(raw_time.begin(), raw_time.end(), current_t - window);
        size_t start_index = std::distance(raw_time.begin(), it);
        size_t data_count = raw_time.size() - start_index;

        if (data_count > 1000) 
        {
            std::vector<Point> temp_tot, temp_c, temp_h;
            temp_tot.reserve(data_count); 
            temp_c.reserve(data_count); 
            temp_h.reserve(data_count);

            for(size_t i = start_index; i < raw_time.size(); ++i) 
            {
                temp_tot.push_back({raw_time[i], raw_tot[i]});
                temp_c.push_back({raw_time[i], raw_carn[i]});
                temp_h.push_back({raw_time[i], raw_herb[i]});
            }

            auto r_tot = resample_curve(temp_tot, 1000);
            auto r_c   = resample_curve(temp_c, 1000);
            auto r_h   = resample_curve(temp_h, 1000);

            view_t   = std::make_shared<std::vector<float>>();
            view_tot = std::make_shared<std::vector<float>>();
            view_c   = std::make_shared<std::vector<float>>();
            view_h   = std::make_shared<std::vector<float>>();

            for(size_t i=0; i<r_tot.size(); ++i) 
            {
                view_t->push_back(r_tot[i].x);
                view_tot->push_back(r_tot[i].y);
                view_c->push_back(r_c[i].y);
                view_h->push_back(r_h[i].y);
            }

            p_time = view_t->data(); 
            p_tot  = view_tot->data(); 
            p_carn = view_c->data(); 
            p_herb = view_h->data();
            count  = view_t->size();
        }
        else 
        {
            p_time = &raw_time[start_index];
            p_tot  = &raw_tot[start_index];
            p_carn = &raw_carn[start_index];
            p_herb = &raw_herb[start_index];
            count  = data_count;
        }
    }

    if (reset_simulation_requested) 
    {
        show_organism_config = false;
        show_organism_population = false;
        show_organism_stats = false;
        real_time_view = true;
        show_all_history = false;
        last_minute_static = false;
        last_5_minutes = false;
        reset_simulation_requested = false;
    }

    ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0.1f, 0.1f));

    if (ImPlot::BeginPlot("Variacion de poblacion", ImVec2(-1, -1)))
    {
        ImPlot::SetupAxes("Tiempo (s)", "Poblacion");
        float current_t = (!raw_time.empty()) ? raw_time.back() : 0.0f;

        if (real_time_view) {
            ImPlot::SetupAxisLimits(ImAxis_X1, current_t - 35.0f, current_t, ImGuiCond_Always);
        } 
        else if (show_all_history && !raw_time.empty()) {
            ImPlot::SetupAxisLimits(ImAxis_X1, raw_time.front(), current_t, ImGuiCond_Always);
        } 
        else if ((last_minute_static || last_5_minutes) && !raw_time.empty()) {
             float window = last_minute_static ? 60.0f : 300.0f;
             ImPlot::SetupAxisLimits(ImAxis_X1, current_t - window, current_t, ImGuiCond_Always);
        }
        
        ImPlot::SetupAxisFormat(ImAxis_Y1, "%.0f");
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);

        if (count > 0) 
        {
            ImPlot::SetNextLineStyle(ImVec4(0.7f, 0.7f, 0.7f, 0.5f), 1.0f);
            ImPlot::PlotLine("Total", p_time, p_tot, count);

            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), 2.0f);
            ImPlot::PlotLine("Carnivoros", p_time, p_carn, count);

            ImPlot::SetNextLineStyle(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), 2.0f);
            ImPlot::PlotLine("Herbivoros", p_time, p_herb, count);
        }

        ImPlot::EndPlot();
    }

    ImPlot::PopStyleVar();

    ImGui::End();
}

void ImPlotMenu::organism_stats_plot()
{
    ImGui::Begin("Estadisticas de Organismos");

    static int selector = 0;
    const char* items[] = {"Velocidad promedio", "Vision promedio", "Tamaño promedio"};
    ImGui::Combo("Seleccione estadistica", &selector, items, IM_ARRAYSIZE(items));

    ImGui::Separator();

    const auto& time_data = PopulationStats::get_time_history();

    // don't do resampling for now, just plot raw data, it is erased every 2000 points

    if (reset_simulation_requested)
    {
        show_organism_config = false;
        show_organism_population = false;
        show_organism_stats = false;
        selector = 0;
        reset_simulation_requested = false;
    }

    ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0.1f, 0.1f));

    if (ImPlot::BeginPlot("Evolucion", ImVec2(-1, -1)))
    {
        ImPlot::SetupAxes("Tiempo (s)", "Valor");
        float current_time = (!time_data.empty()) ? time_data.back() : 0.0f;

        if (!time_data.empty())
        {
            ImPlot::SetupAxisLimits(ImAxis_X1, time_data.front(), current_time, ImGuiCond_Always);
            ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);
        
            if (selector == 0) 
            {
                const auto& speed_data = PopulationStats::organisms_speed_data;
                const auto& carn_speed_data = PopulationStats::carnivores_speed_data;
                const auto& herb_speed_data = PopulationStats::herbivores_speed_data;
                ImPlot::SetNextLineStyle(ImVec4(0, 1, 1, 1), 2.0f); 
                ImPlot::PlotLine("Avg Speed", time_data.data(), speed_data.data(), time_data.size());
                ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1), 2.0f);
                ImPlot::PlotLine("Avg Carn Speed", time_data.data(), carn_speed_data.data(), time_data.size());
                ImPlot::SetNextLineStyle(ImVec4(0, 1, 0, 1), 2.0f);
                ImPlot::PlotLine("Avg Herb Speed", time_data.data(), herb_speed_data.data(), time_data.size());
            }
            else if (selector == 1) 
            {
                const auto& vision_data = PopulationStats::organisms_vision_data;
                const auto& carn_vision_data = PopulationStats::carnivores_vision_data;
                const auto& herb_vision_data = PopulationStats::herbivores_vision_data;
                ImPlot::SetNextLineStyle(ImVec4(1, 0, 1, 1), 2.0f);
                ImPlot::PlotLine("Avg Vision", time_data.data(), vision_data.data(), time_data.size());
                ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1), 2.0f);
                ImPlot::PlotLine("Avg Carn Vision", time_data.data(), carn_vision_data.data(), time_data.size());
                ImPlot::SetNextLineStyle(ImVec4(0, 1, 0, 1), 2.0f);
                ImPlot::PlotLine("Avg Herb Vision", time_data.data(), herb_vision_data.data(), time_data.size());
            }
            else if (selector == 2)
            {
                const auto& size_data = PopulationStats::organisms_size_data;
                const auto& carn_size_data = PopulationStats::carnivores_size_data;
                const auto& herb_size_data = PopulationStats::herbivores_size_data;
                ImPlot::SetNextLineStyle(ImVec4(0, 1, 1, 1), 2.0f);
                ImPlot::PlotLine("Avg Size", time_data.data(), size_data.data(), time_data.size());
                ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1), 2.0f);
                ImPlot::PlotLine("Avg Carn Size", time_data.data(), carn_size_data.data(), time_data.size());
                ImPlot::SetNextLineStyle(ImVec4(0, 1, 0, 1), 2.0f);
                ImPlot::PlotLine("Avg Herb Size", time_data.data(), herb_size_data.data(), time_data.size());
            }

        }
        ImPlot::EndPlot();
    }

    ImPlot::PopStyleVar();

    ImGui::End();
}
