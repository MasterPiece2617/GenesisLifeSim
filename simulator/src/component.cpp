#include <component.hpp>
#include <entity.hpp>

Component::Component(std::weak_ptr<Entity> _owner) :owner(_owner) {}

std::weak_ptr<Entity> Component::get_owner() const
{
	return owner;
}

bool Component::set_owner(std::weak_ptr<Entity> _owner)
{
	if (!_owner.lock())
	{
		return false;
	}

	owner = _owner;
	return true;	
}

void Component::start() {}

void Component::update() {}
 