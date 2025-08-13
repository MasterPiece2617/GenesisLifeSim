#include <entity.hpp>
#include <component.hpp>

Entity::Entity(const std::string& _name) : name(_name)  {}

const std::string& Entity::get_name() const
{
	return name;
}

void Entity::set_name(const std::string& _name)
{
	name = _name;
}

void Entity::print() const
{
	std::cout << "Entity Name: " << name << std::endl;
}

bool Entity::get_is_active() const
{
	return is_active;
}

void Entity::deactivate()
{
	is_active = false;
}

void Entity::activate()
{
	is_active = true;
}

bool Entity::add_component(std::shared_ptr<Component> component)
{
	if (auto self = shared_from_this()) 
	{
		component->set_owner(self);
		components.push_back(component);
		return true;
	}

	return false;
}

Transform& Entity::get_transform() const
{
	if (!transform) {
		throw std::runtime_error("Transform not initialized for entity: " + name);
	}

	return *transform;
}

bool Entity::add_tag(const std::string& tag)
{
	if (std::find(tags.begin(), tags.end(), tag) != tags.end())
	{
		return false;
	}

	tags.push_back(tag);
	return true;
}

bool Entity::has_tag(const std::string& tag) const
{
	return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

bool Entity::remove_tag(const std::string& tag)
{
	auto it = std::find(tags.begin(), tags.end(), tag);
	if (it != tags.end())
	{
		tags.erase(it);
		return true;
	}
	return false;
}

bool Entity::set_parent(std::shared_ptr<Entity> _parent)
{
	return transform->set_parent(_parent);
}

std::shared_ptr<Entity> Entity::get_parent() const
{
	return transform->get_parent();
}

bool Entity::add_child(std::shared_ptr<Entity> child)
{
	if (transform->add_child(child))
	{
		child->set_parent(shared_from_this());
		return true;
	}

	return false;
}

bool Entity::remove_child(std::shared_ptr<Entity> child)
{
	if (transform->remove_child(child))
	{
		return true;
	}

	return false;
}

void Entity::start()
{
	for (auto& component : components)
	{
		component->start();
	}
}

void Entity::update()
{
	for (auto& component : components)
	{
		component->update();
	}
}