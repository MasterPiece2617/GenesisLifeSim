#include <imgui_menu.hpp>

bool is_carnivore = false;

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
    ImGui::Text("Stats del Organismo:");
    ImGui::Separator();

    // --- Tamaño ---
    if (ImGui::SliderFloat("Tamaño (Size)", &organism_config.size, 0.5f, 5.0f))
    {
        // Derivados
        organism_config.weight = std::pow(organism_config.size, 3.0f);
        organism_config.hp = 100.0f * organism_config.size;
        organism_config.max_hunger = 200.0f * organism_config.size;
        organism_config.nu = organism_config.weight;
        organism_config.stamina = 100.0f * organism_config.size;
		organism_config.max_hp = organism_config.hp;
		organism_config.hunger = organism_config.max_hunger * 0.7f;
		organism_config.max_stamina = organism_config.stamina;
        // Clamp de velocidad actual al nuevo rango
        float speed_min = 3.0f + 2.0f * organism_config.size;
        float speed_max = 8.0f + 4.0f * organism_config.size;
        organism_config.move_speed = std::clamp(organism_config.move_speed, speed_min, speed_max);

        // Recalcular rango de visión y clamp
        float vision_base = 5.0f + 2.0f * organism_config.size;
        float vision_min = vision_base * 0.8f;
        float vision_max = vision_base * 1.2f;
        organism_config.vision_radius = std::clamp(organism_config.vision_radius, vision_min, vision_max);
    }

    // --- Velocidad dependiente del tamaño (rango creciente con size) ---
    float speed_min = 3.0f + 2.0f * organism_config.size;
    float speed_max = 8.0f + 4.0f * organism_config.size;
    ImGui::SliderFloat("Velocidad (Speed)", &organism_config.move_speed, speed_min, speed_max);

    // --- Visión dependiente del tamaño con margen ---
    float vision_base = 5.0f + 2.0f * organism_config.size;
    float vision_min = vision_base * 0.8f;
    float vision_max = vision_base * 1.2f;
    ImGui::SliderFloat("Vision", &organism_config.vision_radius, vision_min, vision_max);

    // --- Mostrar valores derivados ---
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

    ImGui::Text("cantidad de organismos a generar");
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
static bool show_carnivore_herbivore = false;


void ImGuiMenu::show_menu(OrganismConfig& organism_config, sf::RenderWindow* window)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Plots"))
        {
            ImGui::MenuItem("Organism Population", NULL, &show_organism_population);
            ImGui::MenuItem("Carnivore vs Herbivore", NULL, &show_carnivore_herbivore);
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

    if (show_carnivore_herbivore)
    {
        ImPlotMenu::carnivore_herbivore_plot();
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

    // raw data, better name
    static std::vector<float> raw_time_data;
    static std::vector<float> raw_pop_data;

    static float time_accumulator = 0.0f;
    time_accumulator += ImGui::GetIO().DeltaTime;

    if (time_accumulator > 0.1f)
    {
        time_accumulator = 0.0f;
        float current_time = ImGui::GetTime();
        int organism_count = PopulationStats::num_organisms;

        raw_time_data.push_back(current_time);
        raw_pop_data.push_back(static_cast<float>(organism_count));

        // 10 min = 600 seg. 10 info/seg = 600 points.
        if (raw_time_data.size() > 60000)
        {
            raw_time_data.erase(raw_time_data.begin());
            raw_pop_data.erase(raw_pop_data.begin());
        }
    }

    ImGui::Begin("Organism Population Over Time Plot");

    if (ImGui::Checkbox("Tiempo Real", &real_time_view)) {
        if(real_time_view) 
        { 
            show_all_history = false; 
            last_minute_static = false; 
            last_5_minutes = false; 
        }
    }
    if (ImGui::Checkbox("A lo largo del tiempo", &show_all_history)) {
        if(show_all_history) 
        { 
            real_time_view = false; 
            last_minute_static = false; 
            last_5_minutes = false; 
        }
    }
    if (ImGui::Checkbox("Pasado 1 miunto", &last_minute_static)) {
        if(last_minute_static) 
        { 
            real_time_view = false; 
            show_all_history = false; 
            last_5_minutes = false; 
        }
    }
    if (ImGui::Checkbox("Pasado 5 minutos", &last_5_minutes)) {
        if(last_5_minutes) 
        { 
            real_time_view = false; 
            show_all_history = false; 
            last_minute_static = false; 
        }
    }

    ImGui::Separator();

    float* plot_x = raw_time_data.data();
    float* plot_y = raw_pop_data.data();
    int plot_count = raw_time_data.size();
    
    // Vectores temporales para resamplear
    std::vector<Point> temp_points;
    std::vector<float> resampled_x;
    std::vector<float> resampled_y;

    if (last_minute_static || last_5_minutes) 
    {
        float current_t = ImGui::GetTime();
        float window_size = last_minute_static ? 60.0f : 300.0f;

        // filter data required
        for(size_t i=0; i<raw_time_data.size(); ++i) {
            if(raw_time_data[i] >= current_t - window_size) {
                temp_points.push_back({raw_time_data[i], raw_pop_data[i]});
            }
        }
        
        // Resampled xd
        if (temp_points.size() > 2) {
            auto resampled = resample_curve(temp_points, 500); // Tu función
            for(const auto& p : resampled) {
                resampled_x.push_back(p.x);
                resampled_y.push_back(p.y);
            }
            plot_x = resampled_x.data();
            plot_y = resampled_y.data();
            plot_count = resampled_x.size();
        }
    }

    if (ImPlot::BeginPlot("Poblacion", ImVec2(-1, -1)))
    {
        ImPlot::SetupAxes("Tiempo (s)", "Poblacion");
        float current_t = ImGui::GetTime();

        if (real_time_view)
        {
            ImPlot::SetupAxisLimits(ImAxis_X1, current_t - 60.0f, current_t, ImGuiCond_Always);
        }
        else if (show_all_history && !raw_time_data.empty())
        {
                ImPlot::SetupAxisLimits(ImAxis_X1, raw_time_data.front(), current_t, ImGuiCond_Always);
        }
        else if (last_minute_static || last_5_minutes)
        {
            float window = last_minute_static ? 60.0f : 300.0f;
            ImPlot::SetupAxisLimits(ImAxis_X1, current_t - window, current_t, ImGuiCond_Always);
        }
 
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);

        if (plot_count > 0)
        {
            ImPlot::PlotLine("Organismos", plot_x, plot_y, plot_count);
        }

        ImPlot::EndPlot();
    }

    ImGui::End();
}

void ImPlotMenu::carnivore_herbivore_plot()
{
    static bool real_time_view = true;
    static bool show_all_history = false;
    static bool last_minute_static = false;
    static bool last_5_minutes = false;

    static std::vector<float> raw_time_data;
    static std::vector<float> raw_carn_data;
    static std::vector<float> raw_herb_data;

    static float time_accumulator = 0.0f;
    time_accumulator += ImGui::GetIO().DeltaTime;

    if (time_accumulator > 0.1f)
    {
        time_accumulator = 0.0f;

        float current_time = ImGui::GetTime();
        int carnivore_count = PopulationStats::num_carnivores;
        int herbivore_count = PopulationStats::num_herbivores;

        raw_time_data.push_back(current_time);
        raw_carn_data.push_back(static_cast<float>(carnivore_count));
        raw_herb_data.push_back(static_cast<float>(herbivore_count));

        if (raw_time_data.size() > 60000) 
        {
             raw_time_data.erase(raw_time_data.begin());
             raw_carn_data.erase(raw_carn_data.begin());
             raw_herb_data.erase(raw_herb_data.begin());
        }
    }

    ImGui::Begin("Carnivore vs Herbivore Plot");

    if (ImGui::Checkbox("View: Real Time", &real_time_view)) {
        if(real_time_view) 
        { 
            show_all_history = false; 
            last_minute_static = false; 
            last_5_minutes = false; 

        }
    }
    if (ImGui::Checkbox("View: All History", &show_all_history)) {
        if(show_all_history) 
        { 
            real_time_view = false; 
            last_minute_static = false; 
            last_5_minutes = false; 

        }
    }
    if (ImGui::Checkbox("View: Last Minute", &last_minute_static)) {
        if(last_minute_static) 
        { 
            real_time_view = false; 
            show_all_history = false; 
            last_5_minutes = false; 

        }
    }
    if (ImGui::Checkbox("View: Last 5 Minutes", &last_5_minutes)) {
        if(last_5_minutes) 
        { 
            real_time_view = false; 
            show_all_history = false; 
            last_minute_static = false; 

        }
    }

    ImGui::Separator();

    float* plot_x_c = raw_time_data.data();
    float* plot_y_c = raw_carn_data.data();
    int plot_count_c = raw_time_data.size();

    float* plot_x_h = raw_time_data.data();
    float* plot_y_h = raw_herb_data.data();
    int plot_count_h = raw_time_data.size();

    std::vector<float> res_time_c, res_val_c;
    std::vector<float> res_time_h, res_val_h;

    if ((last_minute_static || last_5_minutes)) 
    {
        float current_t = ImGui::GetTime();
        float window_size = last_minute_static ? 60.0f : 300.0f;
        
        std::vector<Point> temp_c;
        std::vector<Point> temp_h;

        for(size_t i=0; i<raw_time_data.size(); ++i) 
        {
            if(raw_time_data[i] >= current_t - window_size) {
                temp_c.push_back({raw_time_data[i], raw_carn_data[i]});
                temp_h.push_back({raw_time_data[i], raw_herb_data[i]});
            }
        }
        
        if (temp_c.size() > 2) 
        {
            auto resampled_c = resample_curve(temp_c, 500);
            for(const auto& p : resampled_c) 
            {
                res_time_c.push_back(p.x);
                res_val_c.push_back(p.y);
            }
            plot_x_c = res_time_c.data();
            plot_y_c = res_val_c.data();
            plot_count_c = res_time_c.size();
        }

        if (temp_h.size() > 2) 
        {
            auto resampled_h = resample_curve(temp_h, 500);
            for(const auto& p : resampled_h) 
            {
                res_time_h.push_back(p.x);
                res_val_h.push_back(p.y);
            }
            plot_x_h = res_time_h.data();
            plot_y_h = res_val_h.data();
            plot_count_h = res_time_h.size();
        }
    }

    if (ImPlot::BeginPlot("Carnivore vs Herbivore Over Time", ImVec2(-1, -1)))
    {
        ImPlot::SetupAxes("Time (s)", "Population");
        float current_t = ImGui::GetTime();

        if (real_time_view)
        {
            ImPlot::SetupAxisLimits(ImAxis_X1, current_t - 60.0f, current_t, ImGuiCond_Always);
        }
        else if (show_all_history)
        {
            ImPlot::SetupAxisLimits(ImAxis_X1, raw_time_data.front(), current_t, ImGuiCond_Always);
        }
        else if (last_minute_static || last_5_minutes)
        {
            float window = last_minute_static ? 60.0f : 300.0f;
            ImPlot::SetupAxisLimits(ImAxis_X1, current_t - window, current_t, ImGuiCond_Always);
        }
  
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);

        if (plot_count_c > 0)
        {
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 2.0f);
            ImPlot::PlotLine("Carnivores", plot_x_c, plot_y_c, plot_count_c);
        }

        if (plot_count_h > 0)
        {
            ImPlot::SetNextLineStyle(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 2.0f);
            ImPlot::PlotLine("Herbivores", plot_x_h, plot_y_h, plot_count_h);
        }

        ImPlot::EndPlot();
    }

    ImGui::End();
}