#pragma once

#include <component.hpp>
#include <entity.hpp>
#include <event_manager.hpp>

class Entity; // Forward declaration

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

class EventTest1 : public Component
{
public:
	EventTest1(std::weak_ptr<Entity> _owner) : Component(_owner) {}

	void start() override
	{
		EventCallback callback = [&](const Event& event)
		{
			std::cout << "Entity: " << event.get_data<EntityEvent>().entity->get_name() << " created event received in EventTest1." << std::endl;
		};

		EventManager::suscribe(owner.lock(), EventType::ENTITY_CREATED, callback);
	}
	void update() override
	{

	}

	~EventTest1() override = default;
};