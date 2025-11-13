#pragma once

#include <cstdint>
#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include <component.hpp>
#include <event_manager.hpp>
#include <texture.hpp>
#include <transform.hpp>
#include <entity.hpp>


class SpriteRenderer : public Component
{
	protected:
	sf::VertexArray vertices;
	std::string texture_id;
	sf::IntRect texture_coords;
	uint8_t layer;

	public:
	SpriteRenderer(std::weak_ptr<Entity> _owner, std::string _texture = "error_texture", uint8_t _layer = 0);
	uint8_t get_layer() const;
	void set_texture(std::string _texture_id);
	void build_batch();
	sf::VertexArray& get_batch();
	void start() override;
	void update() override;
	~SpriteRenderer() override = default;
};