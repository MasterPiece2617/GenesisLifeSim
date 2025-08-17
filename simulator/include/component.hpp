#pragma once

#include <iostream>

class Entity; // Forward declaration

class Component
{
protected:
	bool is_active = true;
	std::weak_ptr<Entity> owner;

public:
	Component(std::weak_ptr<Entity> _owner);

	std::weak_ptr<Entity> get_owner() const;
	bool set_owner(std::weak_ptr<Entity> _owner);
	bool get_is_active() const;
	void deactivate();
	void activate();
	virtual void start();
	virtual void update();
	virtual ~Component() = default;
};