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

	static void init(std::shared_ptr<sf::RenderWindow> window);

	// Getters to ImPlot
	static int get_num_organisms();
	static int get_num_carnivores();
	static int get_num_herbivores();

};