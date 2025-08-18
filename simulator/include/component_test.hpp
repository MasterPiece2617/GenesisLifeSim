#pragma once

#include <component.hpp>

class ComponentTest : public Component
{
public:
	
	ComponentTest(std::weak_ptr<Entity> _owner) : Component(_owner) {}

	void start() override
	{
		std::cout << "ComponentTest from " << owner.lock()->get_name() << " started." << std::endl;
	}

	void update() override
	{
		std::cout << "ComponentTest from " << owner.lock()->get_name() << " updated." << std::endl;
	}

	~ComponentTest() override = default;
};