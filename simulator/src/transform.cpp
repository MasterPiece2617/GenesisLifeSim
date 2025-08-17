#include <transform.hpp>

Transform::Transform(std::weak_ptr<Entity> _owner) : Component(_owner) {}

bool Transform::set_parent(std::shared_ptr<Entity> _parent) 
{
	if (!_parent) {
		return false;
	}

	if (_parent == owner.lock())
	{
		std::cerr << "Error: Cannot set owner as its own parent." << std::endl;
		return false;
	}

	auto it = std::find(children.begin(), children.end(), _parent);

	if (it != children.end()) 
	{
		std::cerr << "Error: Cannot set parent to a child entity." << std::endl;
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

	if (child == owner.lock())
	{
		std::cerr << "Error: Cannot add the owner entity as a child." << std::endl;
		return false;
	}

	if (child == parent.lock()) 
	{
		std::cerr << "Error: Cannot add the parent entity as a child." << std::endl;
		return false;
	}

	//child->set_parent(owner.lock());
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

std::vector<std::shared_ptr<Entity>> Transform::get_children() const 
{
	return children;
}