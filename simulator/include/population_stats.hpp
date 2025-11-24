#pragma once

#include <event_manager.hpp>
#include <organism_config.hpp>
#include <organism.hpp>

class PopulationStats
{
private:
	// Callbacks to keep track of organism counts, prefer this to lambdas
	static void on_organism_born(const Event& event);
	static void on_organism_died(const Event& event);
	
public:
	static int num_organisms ;
	static int num_carnivores;
	static int num_herbivores;

    static float total_average_vision;
    static float total_average_speed;
    static float total_average_vision_carnivores;
    static float total_average_speed_carnivores;
    static float total_average_vision_herbivores;
    static float total_average_speed_herbivores;

    static std::vector<float> organisms_vision_data;
    static std::vector<float> organisms_speed_data;
    static std::vector<float> carnivores_vision_data;
    static std::vector<float> carnivores_speed_data;
    static std::vector<float> herbivores_vision_data;
    static std::vector<float> herbivores_speed_data;

	static void init(std::shared_ptr<sf::RenderWindow> window);

	// Getters to ImPlot
	static int get_num_organisms();
	static int get_num_carnivores();
	static int get_num_herbivores();

    static std::vector<float> get_organisms_vision_data();
    static std::vector<float> get_organisms_speed_data();
};