#include <stack>
#include <queue>
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

template<typename Func>
static void child_propagation(std::shared_ptr<Entity> entity, Func func)
{
	std::queue<std::shared_ptr<Entity>> qEntity;
	std::unordered_map<std::shared_ptr<Entity>, bool> visited;

    for (auto& child : entity->get_children())
    {
        if (visited.find(child) != visited.end())
            continue;
        visited[child] = true;
        qEntity.push(child);
    }

    while (!qEntity.empty())
    {
        auto &current = qEntity.front();
        qEntity.pop();
        func(current);

        for (auto& child : current->get_children())
        {
            if (visited.find(child) == visited.end())
            {
                visited[child] = true;
                qEntity.push(child);
            }
        }
    }
}


// Parent-child relationship management
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
	// Children propagation
	const auto &ownered = owner.lock();
	child_propagation(ownered, [&](std::shared_ptr<Entity> child_entity) 
	{
		if (child_entity) 
		{
			auto& child_transform = child_entity->get_transform();
			sf::Vector2f local = child_transform.get_local_position();
			float radians = ownered->get_transform().get_rotation() * 3.14159265f / 180.f;
			sf::Vector2f rotated_local( // Rotate local position by parent's rotation, like the moon around the earth
				local.x * cos(radians) - local.y * sin(radians),
				local.x * sin(radians) + local.y * cos(radians)
			);

			child_transform.set_position(ownered->get_transform().get_position() + rotated_local);
		}
	});
	dirty = true;
}

sf::Vector2f Transform::get_position() const
{
	return position;
}

void Transform::set_scale(const sf::Vector2f &_scale)
{
	scale = _scale;
	// Children propagation
	const auto& ownered = owner.lock();
	child_propagation(ownered, [&](std::shared_ptr<Entity> child_entity) 
	{
		if (child_entity)
		{
			auto& child_transform = child_entity->get_transform();
			child_transform.set_scale(sf::Vector2f(ownered->get_transform().get_scale().x * child_transform.get_local_scale().x,
				ownered->get_transform().get_scale().y * child_transform.get_scale().y));
		}
	});
	dirty = true;
}

sf::Vector2f Transform::get_scale() const
{
	return scale;
}

void Transform::set_rotation(float _angle)
{
	angle = _angle;
	// Children propagation
	const auto& ownered = owner.lock();
	child_propagation(ownered, [&](std::shared_ptr<Entity> child_entity)
		{
			if (child_entity)
			{
				auto& child_transform = child_entity->get_transform();
				child_transform.set_rotation(ownered->get_transform().get_rotation() + child_transform.get_local_rotation());
			}
		});
	dirty = true;
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
void Transform::translate(const sf::Vector2f &offset)
{
	position += offset;
	dirty = true;
}

void Transform::add_scale(const sf::Vector2f &offset)
{
	// Element-wise multiplication for sf::Vector2f
	scale.x *= offset.x;
	scale.y *= offset.y;
	dirty = true;
}

void Transform::rotate(float _angle)
{
	angle += _angle;
	dirty = true;
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

// destructor Transform
Transform::~Transform()
{
	for (auto &child : children)
	{
		if (child)
		{
			remove_child(child);
			child.reset();
		}
	}
	children.clear();

	if (!parent.expired())
	{
		auto p = parent.lock();
		auto &parent_transform = p->get_transform();
		parent_transform.remove_child(owner.lock());
	}
}