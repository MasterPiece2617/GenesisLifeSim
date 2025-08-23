#pragma once

#include <vector>

#include <SFML/System.hpp>

#include <component.hpp>


class Transform : public Component
{
protected:
	sf::Vector2f position = sf::Vector2f(0, 0);
	sf::Vector2f scale = sf::Vector2f(1, 1);
	float angle = 0; // Grades
	std::weak_ptr<Entity> parent;
	std::vector<std::shared_ptr<Entity>> children;

public:

	Transform(std::weak_ptr<Entity> _owner);
	std::shared_ptr<Entity> get_owner() const;
	bool set_parent(std::shared_ptr<Entity> _parent);
	std::shared_ptr<Entity> get_parent() const;
	bool add_child(std::shared_ptr<Entity> child);
	bool remove_child(std::shared_ptr<Entity> child);
	std::vector<std::shared_ptr<Entity>> get_children() const;

	void set_position(const sf::Vector2f& pos);
	sf::Vector2f get_position() const;
	void set_scale(const sf::Vector2f& _scale);
	sf::Vector2f get_scale() const;
	void set_rotation(float _angle);
	float get_rotation() const;

	void set_local_position(const sf::Vector2f& pos);
	sf::Vector2f get_local_position() const;
	void set_local_scale(const sf::Vector2f& _scale);
	sf::Vector2f get_local_scale() const;
	void set_local_rotation(float _angle);
	float get_local_rotation() const;

	void translate_mod(const sf::Vector2f& offset);
	void rotate_mod(float _angle);
	void scale_mod(const sf::Vector2f& offset);

	sf::Vector2f get_forward() const;
	void look_at(const sf::Vector2f& target);


	~Transform() override = default;
};