#include <population_stats.hpp>

int PopulationStats::num_organisms = 0;
int PopulationStats::num_carnivores = 0;
int PopulationStats::num_herbivores = 0;

float PopulationStats::total_average_speed = 0.0f;
float PopulationStats::total_average_vision = 0.0f;
float PopulationStats::total_average_speed_carnivores = 0.0f;
float PopulationStats::total_average_vision_carnivores = 0.0f;
float PopulationStats::total_average_speed_herbivores = 0.0f;
float PopulationStats::total_average_vision_herbivores = 0.0f;

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
            total_average_vision_carnivores = 0.0f;
            total_average_speed_carnivores = 0.0f;
            total_average_vision_herbivores = 0.0f;
            total_average_speed_herbivores = 0.0f;
        }
    }
}

std::vector<float> PopulationStats::get_organisms_vision_data()
{
    return organisms_vision_data;
}

std::vector<float> PopulationStats::get_organisms_speed_data()
{
    return organisms_speed_data;
}