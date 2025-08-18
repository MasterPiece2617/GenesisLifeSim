#pragma once

#include <entity.hpp>
#include <component_test.hpp>

class EntityTest : public Entity
{
public:
	EntityTest(const std::string& _name) : Entity(_name){}

	void init() override
	{
		add_component(std::make_shared<ComponentTest>(shared_from_this()));
		add_component(std::make_shared<ComponentTest>(shared_from_this()));
	}

	void print() const override
	{
		Entity::print();
		std::cout << "This is an test" << std::endl;
	}

	~EntityTest() override = default;
};