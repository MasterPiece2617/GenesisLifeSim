#pragma once

#include <vector>

#include <SFML/System.hpp>

#include <component.hpp>


class Transform : public Component
{
protected:
	sf::Vector2f position;
	sf::Vector2f scale;
	sf::Vector2f rotation; // Grades
	std::weak_ptr<Entity> parent;
	std::vector<std::shared_ptr<Entity>> children;

public:

	Transform(std::weak_ptr<Entity> _owner);
	bool set_parent(std::shared_ptr<Entity> _parent);
	std::shared_ptr<Entity> get_parent() const;
	bool add_child(std::shared_ptr<Entity> child);
	bool remove_child(std::shared_ptr<Entity> child);
};