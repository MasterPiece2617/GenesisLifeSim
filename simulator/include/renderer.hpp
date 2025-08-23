#pragma once

#include <cstdint>
#include <memory>

#include <SFML/Graphics.hpp>

#include <component.hpp>
#include <event_manager.hpp>

class Renderer : public Component
{
	protected:
	std::shared_ptr<sf::Drawable> object;
	uint8_t layer;

	public:
	Renderer() = default;
	uint8_t get_layer() const;
	std::shared_ptr<sf::Drawable> get_object() const;
	void start() override;
	void update() override;
	~Renderer() override = default;
};