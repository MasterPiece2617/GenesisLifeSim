#include <stack>
#include <unordered_map>

#include <transform.hpp>
#include <entity.hpp>

Transform::Transform(std::weak_ptr<Entity> _owner) : Component(_owner) {}


static bool search_valid_parent(const std::shared_ptr<Entity>& poss_parent, const std::shared_ptr<Entity>& owner)
{
	// DFS to check for cycles and indirect parent-child relationships
	std::stack<std::shared_ptr<Entity>> parent_stack;
	std::unordered_map<std::shared_ptr<Entity>, bool> visited;

	parent_stack.push(poss_parent);
	while (!parent_stack.empty())
	{
		auto current = parent_stack.top();
		parent_stack.pop();
		if (visited.find(current) != visited.end())
		{
			continue; // Already visited
		}
		visited[current] = true;
		if (current == owner)
		{
			return false; // Found a cycle
		}
		for (const auto& child : current->get_children())
		{
			if (child == owner)
			{
				return false; // poss_parent is already a parent of owner
			}
			parent_stack.push(child);
		}
	}

	// Check if poss_parent is a parent of any ancestor of owner
	auto ancestor = owner->get_parent();
	while (ancestor)
	{
		if (ancestor == poss_parent)
		{
			return false; // poss_parent is an ancestor of owner
		}
		ancestor = ancestor->get_parent();
	}

	return true; // No cycle or indirect parent-child relationship found
}

bool Transform::set_parent(std::shared_ptr<Entity> _parent) 
{
	if (!_parent) 
	{
		return false;
	}

	auto it = std::find(children.begin(), children.end(), _parent);

	if (it != children.end()) 
	{
		std::cerr << "Error: Cannot set parent to a child entity." << std::endl;
		return false;
	}

	if (!search_valid_parent(_parent, owner.lock()))
	{
		std::cerr << "Error: Setting this parent would create a cycle in the hierarchy." << std::endl;
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

	child->set_parent(owner.lock());
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


// Setting transform
void Transform::set_position(const sf::Vector2f &pos)
{
	position = pos;
}

sf::Vector2f Transform::get_position() const
{
	return position;
}

void Transform::set_scale(const sf::Vector2f &_scale)
{
	scale = _scale;
}

sf::Vector2f Transform::get_scale() const
{
	return scale;
}

void Transform::set_rotation(float _angle)
{
	angle = _angle;
}

float Transform::get_rotation() const
{
	return angle;
}

// Setting local transform (relative transform with parent)
void Transform::set_local_position(const sf::Vector2f &pos)
{
	
	if (!parent.expired())
	{
		auto &parent_transform = parent.lock()->get_transform();
		position = pos + parent_transform.get_position();
	}
	else
	{
		position = pos;
	}
}

sf::Vector2f Transform::get_local_position() const
{
	if (!parent.expired())
	{
		auto p = parent.lock();
		auto &parent_transform = p->get_transform();
		return position - parent_transform.get_position();
	}

	return position;
}

void Transform::set_local_scale(const sf::Vector2f &_scale)
{
	
	if (auto p = parent.lock())
	{
		auto &parent_transform = p->get_transform();
        sf::Vector2f parent_scale = parent_transform.get_scale();
        scale = sf::Vector2f(_scale.x * parent_scale.x, _scale.y * parent_scale.y);
	}
	else
	{
		scale = _scale;
	}
}

sf::Vector2f Transform::get_local_scale() const
{
	if (auto p = parent.lock())
	{
		auto &parent_transform = p->get_transform();
		sf::Vector2f parent_scale = parent_transform.get_scale();
		return sf::Vector2f(scale.x / parent_scale.x, scale.y / parent_scale.y);
	}

	return scale;
}

void Transform::set_local_rotation(float _angle)
{
	if (auto p = parent.lock())
	{
		auto &parent_transform = p->get_transform();
		angle = _angle + parent_transform.get_rotation();
	}
	else
	{
		angle = _angle;
	}
}

float Transform::get_local_rotation() const
{
	if (auto p = parent.lock())
	{
		auto &parent_transform = p->get_transform();
		return angle - parent_transform.get_rotation();
	}

	return angle;
}

// Modifying transform
void Transform::translate_mod(const sf::Vector2f &offset)
{
	position += offset;
}

void Transform::scale_mod(const sf::Vector2f &offset)
{
	// Element-wise multiplication for sf::Vector2f
	scale.x *= offset.x;
	scale.y *= offset.y;
}

void Transform::rotate_mod(float _angle)
{
	angle += _angle;
}

// Getting forward vector
sf::Vector2f Transform::get_forward() const
{
	float radians = get_rotation() * 3.14159265f / 180.f; // Convert to radians to normalize
	return sf::Vector2f(cos(radians), sin(radians));
}

void Transform::look_at(const sf::Vector2f &target)
{
	sf::Vector2f direction = target - position;
	angle = atan2(direction.y, direction.x) * 180.f / 3.14159265f; // Convert to degrees to rotate
}