#pragma once  

#include <cmath>  
#include <numbers>  

#include <SFML/System.hpp>  

class Time  
{  
protected:  

	static sf::Time delta_time;  
	static sf::Clock clock;  

public:  

	static void update();  

	static float get_delta();  
};  

class Constants  
{  
public:  
	static const int px_mt = 32; // pixels per meter  
	static const int map_size = 4096;  
	static const int chunk_size = 4;  

	static constexpr float pi_val = 3.1415927f;
};