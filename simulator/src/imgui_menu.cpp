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
    ImGui::Text("Seleccione las caracteristicas que desea cargar en la simulacion.");
    ImGui::Separator();
    ImGui::Text("Stats del Organismo:");
    ImGui::SliderFloat("Vision", &organism_config.vision_radius, 1, 20);
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
	ImGui::Text("Es carnivoro:");
	ImGui::Checkbox("Carnivoro", &is_carnivore);

    if (is_carnivore)
    {
        organism_config.category = OrganismCategory::CARNIVORE;
    }
    else
    {
        organism_config.category = OrganismCategory::HERBIVORE;
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
static bool show_organism_stats = false;


void ImGuiMenu::show_menu(OrganismConfig& organism_config, sf::RenderWindow* window)
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
    static std::vector<float> raw_time;
    static std::vector<float> raw_tot;
    static std::vector<float> raw_carn;
    static std::vector<float> raw_herb;

    static bool real_time_view = true;
    static bool show_all_history = false;
    static bool last_minute_static = false;
    static bool last_5_minutes = false;

    static float time_acc = 0.0f;
    time_acc += ImGui::GetIO().DeltaTime;

    if (time_acc > 0.1f)
    {
        time_acc = 0.0f;
        float t = ImGui::GetTime();

        int total = PopulationStats::num_organisms;
        int carn = PopulationStats::num_carnivores;
        int herb = PopulationStats::num_herbivores;

        raw_time.push_back(t);
        raw_tot.push_back((float)total);
        raw_carn.push_back((float)carn);
        raw_herb.push_back((float)herb);

        if (raw_time.size() > 200000) { 
             raw_time.erase(raw_time.begin());
             raw_tot.erase(raw_tot.begin());
             raw_carn.erase(raw_carn.begin());
             raw_herb.erase(raw_herb.begin());
        }
    }

    ImGui::Begin("Poblacion de Organismos");

    if (ImGui::Checkbox("Tiempo Real", &real_time_view)) {
        if(real_time_view) 
        { 
            show_all_history=false; 
            last_minute_static=false; 
            last_5_minutes=false; 
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Toda la Historia", &show_all_history)) {
        if(show_all_history) 
        { 
            real_time_view=false; 
            last_minute_static=false; 
            last_5_minutes=false; 
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Ultimo Minuto", &last_minute_static)) {
        if(last_minute_static) 
        { 
            real_time_view=false; 
            show_all_history=false; 
            last_5_minutes=false; 
        }
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Ultimos 5 Min", &last_5_minutes)) {
        if(last_5_minutes) 
        { 
            real_time_view=false; 
            show_all_history=false; 
            last_minute_static=false; 
        }
    }

    ImGui::Separator();

    std::shared_ptr<std::vector<float>> view_time_c = nullptr;
    std::shared_ptr<std::vector<float>> view_val_c  = nullptr;
    
    std::shared_ptr<std::vector<float>> view_time_h = nullptr;
    std::shared_ptr<std::vector<float>> view_val_h  = nullptr;
    
    std::shared_ptr<std::vector<float>> view_time_t = nullptr;
    std::shared_ptr<std::vector<float>> view_val_t  = nullptr;

    float* p_time_c = raw_time.data(); float* p_val_c = raw_carn.data(); int count_c = raw_time.size();
    float* p_time_h = raw_time.data(); float* p_val_h = raw_herb.data(); int count_h = raw_time.size();
    float* p_time_t = raw_time.data(); float* p_val_t = raw_tot.data();  int count_t = raw_time.size();

    if (!real_time_view && !raw_time.empty())
    {
        float current_t = ImGui::GetTime();
        size_t start_index = 0;

        if (show_all_history) {
            start_index = 0;
        } 
        else {
            float window = last_minute_static ? 60.0f : 300.0f;
            auto it = std::lower_bound(raw_time.begin(), raw_time.end(), current_t - window);
            start_index = std::distance(raw_time.begin(), it);
        }

        size_t data_count = raw_time.size() - start_index;
        
        if (data_count > 1000) 
        {
            std::vector<Point> temp_tot, temp_c, temp_h;
            temp_tot.reserve(data_count);
            temp_c.reserve(data_count);
            temp_h.reserve(data_count);

            for(size_t i = start_index; i < raw_time.size(); ++i) {
                temp_tot.push_back({raw_time[i], raw_tot[i]});
                temp_c.push_back({raw_time[i], raw_carn[i]});
                temp_h.push_back({raw_time[i], raw_herb[i]});
            }

            auto r_tot = resample_curve(temp_tot, 1000);
            auto r_c   = resample_curve(temp_c, 1000);
            auto r_h   = resample_curve(temp_h, 1000);

            view_time_t = std::make_shared<std::vector<float>>(); view_val_t = std::make_shared<std::vector<float>>();
            view_time_c = std::make_shared<std::vector<float>>(); view_val_c = std::make_shared<std::vector<float>>();
            view_time_h = std::make_shared<std::vector<float>>(); view_val_h = std::make_shared<std::vector<float>>();

            for(const auto& p : r_tot) { view_time_t->push_back(p.x); view_val_t->push_back(p.y); }
            for(const auto& p : r_c)   { view_time_c->push_back(p.x); view_val_c->push_back(p.y); }
            for(const auto& p : r_h)   { view_time_h->push_back(p.x); view_val_h->push_back(p.y); }

            p_time_t = view_time_t->data(); p_val_t = view_val_t->data(); count_t = view_time_t->size();
            p_time_c = view_time_c->data(); p_val_c = view_val_c->data(); count_c = view_time_c->size();
            p_time_h = view_time_h->data(); p_val_h = view_val_h->data(); count_h = view_time_h->size();
        }
        else 
        {
            p_time_t = &raw_time[start_index]; p_val_t = &raw_tot[start_index];
            p_time_c = &raw_time[start_index]; p_val_c = &raw_carn[start_index];
            p_time_h = &raw_time[start_index]; p_val_h = &raw_herb[start_index];
            count_t = count_c = count_h = data_count;
        }
    }

    if (ImPlot::BeginPlot("Variacion de poblacion", ImVec2(-1, -1)))
    {
        ImPlot::SetupAxes("Tiempo (s)", "Poblacion");
        float current_t = ImGui::GetTime();

        if (real_time_view) {
            ImPlot::SetupAxisLimits(ImAxis_X1, current_t - 60.0f, current_t, ImGuiCond_Always);
        } 
        else if (show_all_history && !raw_time.empty()) {
            ImPlot::SetupAxisLimits(ImAxis_X1, raw_time.front(), current_t, ImGuiCond_Always);
        } 
        else if ((last_minute_static || last_5_minutes) && !raw_time.empty()) {
             float window = last_minute_static ? 60.0f : 300.0f;
             ImPlot::SetupAxisLimits(ImAxis_X1, current_t - window, current_t, ImGuiCond_Always);
        }
        
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);

        if (count_t > 0) {
            ImPlot::SetNextLineStyle(ImVec4(0.7f, 0.7f, 0.7f, 0.5f), 1.0f);
            ImPlot::PlotLine("Total", p_time_t, p_val_t, count_t);

            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), 2.0f);
            ImPlot::PlotLine("Carnivoros", p_time_c, p_val_c, count_c);

            ImPlot::SetNextLineStyle(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), 2.0f);
            ImPlot::PlotLine("Herbivoros", p_time_h, p_val_h, count_h);
        }

        ImPlot::EndPlot();
    }

    ImGui::End();
}

void ImPlotMenu::organism_stats_plot()
{
    static float time_accumulator = 0.0f;
    static std::vector<float> time_data;

    time_accumulator += ImGui::GetIO().DeltaTime;

    if (time_accumulator > 0.1f)
    {
        time_accumulator = 0.0f;
        float current_time = ImGui::GetTime();

        float avg_speed = 0.0f;
        float avg_vision = 0.0f;
        float avg_carn_speed = 0.0f;
        float avg_carn_vision = 0.0f;
        float avg_herb_speed = 0.0f;
        float avg_herb_vision = 0.0f;


        if (PopulationStats::num_organisms > 0) 
        {
            avg_speed = PopulationStats::total_average_speed / PopulationStats::num_organisms;
            avg_vision = PopulationStats::total_average_vision / PopulationStats::num_organisms;
            avg_carn_speed = PopulationStats::total_average_speed_carnivores / PopulationStats::num_carnivores;
            avg_carn_vision = PopulationStats::total_average_vision_carnivores / PopulationStats::num_carnivores;
            avg_herb_speed = PopulationStats::total_average_speed_herbivores / PopulationStats::num_herbivores;
            avg_herb_vision = PopulationStats::total_average_vision_herbivores / PopulationStats::num_herbivores;
        }

        time_data.push_back(current_time);
        PopulationStats::organisms_speed_data.push_back(avg_speed);
        PopulationStats::organisms_vision_data.push_back(avg_vision);
        PopulationStats::carnivores_speed_data.push_back(avg_carn_speed);
        PopulationStats::carnivores_vision_data.push_back(avg_carn_vision);
        PopulationStats::herbivores_speed_data.push_back(avg_herb_speed);
        PopulationStats::herbivores_vision_data.push_back(avg_herb_vision);

        if (time_data.size() > 2000) 
        {
            time_data.erase(time_data.begin());
            PopulationStats::organisms_speed_data.erase(PopulationStats::organisms_speed_data.begin());
            PopulationStats::organisms_vision_data.erase(PopulationStats::organisms_vision_data.begin());
            PopulationStats::carnivores_speed_data.erase(PopulationStats::carnivores_speed_data.begin());
            PopulationStats::carnivores_vision_data.erase(PopulationStats::carnivores_vision_data.begin());
            PopulationStats::herbivores_speed_data.erase(PopulationStats::herbivores_speed_data.begin());
            PopulationStats::herbivores_vision_data.erase(PopulationStats::herbivores_vision_data.begin());
        }
    }

    ImGui::Begin("Estadisticas de Organismos");

    static int selector = 0;
    const char* items[] = {"Velocidad promedio", "Vision promedio"};
    ImGui::Combo("Seleccione estadistica", &selector, items, IM_ARRAYSIZE(items));

    ImGui::Separator();

    // don't do resampling for now, just plot raw data, it is erased every 2000 points
    if (ImPlot::BeginPlot("Evolucion", ImVec2(-1, -1)))
    {
        ImPlot::SetupAxes("Tiempo (s)", "Valor");

        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_AutoFit);

        if (!time_data.empty())
        {
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
        }

        ImPlot::EndPlot();
    }

    ImGui::End();
}
