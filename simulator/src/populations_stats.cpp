#include <population_stats.hpp>

int PopulationStats::num_organisms = 0;
int PopulationStats::num_carnivores = 0;
int PopulationStats::num_herbivores = 0;

void PopulationStats::init(std::shared_ptr<sf::RenderWindow> window)
{
    // Subscribe to organism born and died events, callback is the function
    EventManager::suscribe(window, EventType::ORGANISM_BORN, PopulationStats::on_organism_born);
    EventManager::suscribe(window, EventType::ORGANISM_DIED, PopulationStats::on_organism_died);
}

void PopulationStats::on_organism_born(const Event& event)
{
    auto curremt_organism = event.get_data<EntityEvent>().entity;

    if (auto organism = std::dynamic_pointer_cast<Organism>(curremt_organism))
    {
        ++num_organisms;

        if ((int)organism->get_stats().category == 1) // carnivore
        {
            ++num_carnivores;
        }
        else if ((int)organism->get_stats().category == 0) // herbivore
        {
            ++num_herbivores;
        }
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
        }
        else if ((int)organism->get_stats().category == 0) // herbivore
        {
            --num_herbivores;
        }
    }
}

