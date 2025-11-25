#include <population_stats.hpp>

float PopulationStats::time = 0.0f;
int PopulationStats::num_organisms = 0;
int PopulationStats::num_carnivores = 0;
int PopulationStats::num_herbivores = 0;

float PopulationStats::total_average_speed = 0.0f;
float PopulationStats::total_average_vision = 0.0f;
float PopulationStats::total_average_speed_carnivores = 0.0f;
float PopulationStats::total_average_vision_carnivores = 0.0f;
float PopulationStats::total_average_speed_herbivores = 0.0f;
float PopulationStats::total_average_vision_herbivores = 0.0f;

std::vector<float> PopulationStats::time_history;
std::vector<float> PopulationStats::total_organisms_history;
std::vector<float> PopulationStats::carnivores_history;
std::vector<float> PopulationStats::herbivores_history;
std::vector<float> PopulationStats::organisms_vision_data;
std::vector<float> PopulationStats::organisms_speed_data;
std::vector<float> PopulationStats::carnivores_vision_data;
std::vector<float> PopulationStats::carnivores_speed_data;
std::vector<float> PopulationStats::herbivores_vision_data;
std::vector<float> PopulationStats::herbivores_speed_data;

void PopulationStats::init(std::shared_ptr<sf::RenderWindow> window)
{
    // Subscribe to organism born and died events, callback is the function
    EventManager::suscribe(window, EventType::ORGANISM_BORN, PopulationStats::on_organism_born);
    EventManager::suscribe(window, EventType::ORGANISM_DIED, PopulationStats::on_organism_died);
}

void PopulationStats::reset()
{
    num_organisms = 0;
    num_carnivores = 0;
    num_herbivores = 0;

    total_average_speed = 0.0f;
    total_average_vision = 0.0f;
    total_average_speed_carnivores = 0.0f;
    total_average_vision_carnivores = 0.0f;
    total_average_speed_herbivores = 0.0f;
    total_average_vision_herbivores = 0.0f;

    time = 0.0f;
    time_history.clear();
    total_organisms_history.clear();
    carnivores_history.clear();
    herbivores_history.clear();
    organisms_vision_data.clear();
    organisms_speed_data.clear();
    carnivores_vision_data.clear();
    carnivores_speed_data.clear();
    herbivores_vision_data.clear();
    herbivores_speed_data.clear();
}

void PopulationStats::update_history(float delta)
{
    time += delta;

    float avg_spd = (num_organisms > 0) ? (total_average_speed  / num_organisms) : 0.0f; // Avoid division by zero better form to write it
    float avg_vis = (num_organisms > 0) ? (total_average_vision / num_organisms) : 0.0f;

    float avg_spd_c = (num_carnivores > 0) ? (total_average_speed_carnivores / num_carnivores) : 0.0f;
    float avg_vis_c = (num_carnivores > 0) ? (total_average_vision_carnivores / num_carnivores) : 0.0f;

    float avg_spd_h = (num_herbivores > 0) ? (total_average_speed_herbivores / num_herbivores) : 0.0f;
    float avg_vis_h = (num_herbivores > 0) ? (total_average_vision_herbivores / num_herbivores) : 0.0f;

    time_history.push_back(time); // Same time to all plots

    total_organisms_history.push_back(static_cast<float>(num_organisms));
    carnivores_history.push_back(static_cast<float>(num_carnivores));
    herbivores_history.push_back(static_cast<float>(num_herbivores));

    organisms_speed_data.push_back(avg_spd);
    carnivores_speed_data.push_back(avg_spd_c);
    herbivores_speed_data.push_back(avg_spd_h);

    organisms_vision_data.push_back(avg_vis);
    carnivores_vision_data.push_back(avg_vis_c);
    herbivores_vision_data.push_back(avg_vis_h);

    if (time_history.size() > 50000) 
    {
        time_history.erase(time_history.begin());
        
        total_organisms_history.erase(total_organisms_history.begin());
        carnivores_history.erase(carnivores_history.begin());
        herbivores_history.erase(herbivores_history.begin());

        organisms_speed_data.erase(organisms_speed_data.begin());
        carnivores_speed_data.erase(carnivores_speed_data.begin());
        herbivores_speed_data.erase(herbivores_speed_data.begin());

        organisms_vision_data.erase(organisms_vision_data.begin());
        carnivores_vision_data.erase(carnivores_vision_data.begin());
        herbivores_vision_data.erase(herbivores_vision_data.begin());
    }
}

void PopulationStats::on_organism_born(const Event& event)
{
    auto current_organism = event.get_data<EntityEvent>().entity;

    if (auto organism = std::dynamic_pointer_cast<Organism>(current_organism))
    {
        ++num_organisms;

        if ((int)organism->get_stats().category == 1) // carnivore
        {
            ++num_carnivores;
            total_average_speed_carnivores += organism->get_stats().speed;
            total_average_vision_carnivores += organism->get_stats().vision;
        }
        else if ((int)organism->get_stats().category == 0) // herbivore
        {
            ++num_herbivores;
            total_average_speed_herbivores += organism->get_stats().speed;
            total_average_vision_herbivores += organism->get_stats().vision;
        }

        total_average_vision += organism->get_stats().vision;
        total_average_speed += organism->get_stats().speed;
    }
}

void PopulationStats::on_organism_died(const Event& event)
{
    auto total_organism = event.get_data<EntityEvent>().entity;

    if (auto organism = std::dynamic_pointer_cast<Organism>(total_organism))
    {
        --num_organisms;

        if ((int)organism->get_stats().category == 1) // carnivore
        {
            --num_carnivores;
            total_average_vision_carnivores -= organism->get_stats().vision;
            total_average_speed_carnivores -= organism->get_stats().speed;
        }
        else if ((int)organism->get_stats().category == 0) // herbivore
        {
            --num_herbivores;
            total_average_vision_herbivores -= organism->get_stats().vision;
            total_average_speed_herbivores -= organism->get_stats().speed;
        }

        total_average_vision -= organism->get_stats().vision;
        total_average_speed -= organism->get_stats().speed;

        if (num_organisms <= 0)
        {
            total_average_vision = 0.0f;
            total_average_speed = 0.0f;
        }

        if (num_carnivores <= 0)
        {
            total_average_vision_carnivores = 0.0f;
            total_average_speed_carnivores = 0.0f;
        }

        if (num_herbivores <= 0)
        {
            total_average_vision_herbivores = 0.0f;
            total_average_speed_herbivores = 0.0f;
        }
    }
}


// Getters for ImPlot
float PopulationStats::get_time()
{
    return time;
}

std::vector<float> PopulationStats::get_organisms_vision_data()
{
    return organisms_vision_data;
}

std::vector<float> PopulationStats::get_organisms_speed_data()
{
    return organisms_speed_data;
}

std::vector<float> PopulationStats::get_carnivores_vision_data()
{
    return carnivores_vision_data;
}

std::vector<float> PopulationStats::get_carnivores_speed_data()
{
    return carnivores_speed_data;
}

std::vector<float> PopulationStats::get_herbivores_vision_data()
{
    return herbivores_vision_data;
}

std::vector<float> PopulationStats::get_herbivores_speed_data()
{
    return herbivores_speed_data;
}

std::vector<float> PopulationStats::get_time_history()
{
    return time_history;
}

std::vector<float> PopulationStats::get_total_organisms_history()
{
    return total_organisms_history;
}

std::vector<float> PopulationStats::get_carnivores_history()
{
    return carnivores_history;
}

std::vector<float> PopulationStats::get_herbivores_history()
{
    return herbivores_history;
}
