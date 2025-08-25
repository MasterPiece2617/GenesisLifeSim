#include <event_manager.hpp>

Event::Event(EventType event_type) : type(event_type), data(EmptyEvent()) {}

Event::Event(EventType event_type, EventData _data) : type(event_type), data(_data) {}

EventCategory Event::get_event_category() const
{
	if (type < EventTypeInfo::custom_end)
	{
		return EventCategory::SYSTEM_EVENT;
	}
	else if (type < EventTypeInfo::input_end)
	{
		return EventCategory::INPUT_EVENT;
	}
	else if (type < EventTypeInfo::physics_end)
	{
		return EventCategory::PHYSICS_EVENT;
	}
	else if (type < EventTypeInfo::entity_end)
	{
		return EventCategory::ENTITY_EVENT;
	}
	else if (type < EventTypeInfo::custom_end)
	{
		return EventCategory::CUSTOM_DIRECT_EVENT;
	}
	else
	{
		return EventCategory::CUSTOM_GLOBAL_EVENT;
	}
}

EventType Event::get_event_type() const
{
	return type;
}

EventBus EventManager::event_bus;

EventManager::EventManager() {}

void EventManager::suscribe(Actor self, EventType event_type, EventCallback callback)
{
	event_bus[static_cast<size_t>(event_type)][self] = callback;
}

bool EventManager::desuscribe(Actor self, EventType event_type)
{
	auto& callbacks = event_bus[static_cast<size_t>(event_type)];
	auto it = callbacks.find(self);

	if (it != callbacks.end())
	{
		callbacks.erase(it);
		return true;
	}

	return false;
}

void EventManager::publish(const Event& event)
{
	switch (event.get_event_category())
	{
		case EventCategory::SYSTEM_EVENT:
		case EventCategory::INPUT_EVENT:
		case EventCategory::ENTITY_EVENT:
		case EventCategory::CUSTOM_GLOBAL_EVENT:

			for (const auto& callback : event_bus[static_cast<size_t>(event.get_event_type())])
			{
				callback.second(event);
			}

			break;

		default:
			std::cerr << "Error: Event type does need target argument." << std::endl;
			break;
	}
}

void EventManager::publish(const Event& event, Actor target)
{
	switch (event.get_event_category())
	{
		case EventCategory::SYSTEM_EVENT:
		case EventCategory::INPUT_EVENT:
		case EventCategory::ENTITY_EVENT:
		case EventCategory::CUSTOM_GLOBAL_EVENT:

			for (const auto& callback : event_bus[static_cast<size_t>(event.get_event_type())])
			{
				callback.second(event);
			}

			break;

		case EventCategory::PHYSICS_EVENT:
		case EventCategory::CUSTOM_DIRECT_EVENT:

			auto& callbacks = event_bus[static_cast<size_t>(event.get_event_type())];
			auto it = callbacks.find(target);

			if (it != callbacks.end()) 
			{
				it->second(event);
			}

			break;
	}
}

EntityEvent::EntityEvent(const std::shared_ptr<Entity>& entity_ptr) : entity(entity_ptr) {}

MouseWheelEvent::MouseWheelEvent(float _delta) : delta(_delta) {}

float InputManager::scroll_delta;

void InputManager::init()
{
	EventManager::suscribe(std::shared_ptr<Entity>(nullptr), EventType::MOUSE_WHEEL_SCROLLED, [&](const Event& event)
		{
			scroll_delta = event.get_data<MouseWheelEvent>().delta;
		});
}

void InputManager::update()
{
	scroll_delta = 0.0f;
}

float InputManager::get_scroll_delta()
{
	return scroll_delta;
}

