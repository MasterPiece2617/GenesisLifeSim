#include "transform.hpp"

Transform::Transform(std::weak_ptr<Entity> _owner) : Component(_owner) {}

bool Transform::set_parent(std::shared_ptr<Entity> _parent) 
{
	if (!_parent) {
		return false;
	}

	parent = _parent;
	return true;
}

std::shared_ptr<Entity> Transform::get_parent() const 
{
	return parent.lock();
}

bool Transform::add_child(std::shared_ptr<Entity> child) 
{
	if (!child) 
	{
		return false;
	}

	children.push_back(child);
	return true;
}

bool Transform::remove_child(std::shared_ptr<Entity> child) 
{
	auto it = std::find(children.begin(), children.end(), child);

	if (it != children.end()) 
	{
		children.erase(it);
		return true;
	}

	return false;
}