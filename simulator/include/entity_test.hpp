#pragma once

#include <entity.hpp>
#include <component_test.hpp>

class EntityTest : public Entity
{
public:
	EntityTest(const std::string& _name) : Entity(_name)
	{

	}

	void start() override
	{
		add_component(std::make_shared<ComponentTest>());
		std::cout << "EntityTest " << name << " started." << std::endl;
		Entity::start();
	}

	void update() override
	{
		std::cout << "EntityTest " << name << " updated." << std::endl;
		Entity::update();
	}

};