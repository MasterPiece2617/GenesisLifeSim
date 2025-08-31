#pragma once

#include <iostream>
#include <filesystem>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include <utils.hpp>

class Atlas
{

protected:

    std::unordered_map<std::string, sf::IntRect> regions;
    std::unique_ptr<sf::Texture> atlas = nullptr;

public:

    Atlas();
    const std::unordered_map<std::string, sf::IntRect>& get_regions();
    const sf::Texture& get_atlas();
};

class Texture
{
protected:

    static std::unique_ptr<Atlas> atlas;

public:

	static sf::IntRect get_region(const std::string& name);
    static const sf::Texture& get_atlas();
};