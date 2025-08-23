#pragma once

#include <cstdint>
#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include <component.hpp>
#include <event_manager.hpp>

class Renderer : public Component
{
	protected:
	std::shared_ptr<sf::Drawable> object;
	std::shared_ptr<sf::Texture> texture;
	uint8_t layer;

	static std::shared_ptr<sf::Texture> error_texture;
	static void set_error_texture();

	public:
	Renderer(std::weak_ptr<Entity> _owner, std::shared_ptr<sf::Drawable> _object = nullptr, std::shared_ptr<sf::Texture> _texture = std::make_shared<sf::Texture>(), uint8_t _layer = 0);
	uint8_t get_layer() const;
	std::shared_ptr<sf::Drawable> get_object() const;
	void change_texture(std::shared_ptr<sf::Texture> new_texture);
	void start() override;
	void update() override;
	~Renderer() override = default;
};