#pragma once

#include <array>
#include <functional>
#include <memory>
#include <unordered_map>
#include <variant>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <entity.hpp>

class Entity; // Forward declaration

enum class EventType
{
	// System events
	WINDOW_CLOSED,
	WINDOW_RESIZED,
	WINDOW_LOST_FOCUS,
	WINDOW_GAINED_FOCUS,
	SYSTEM_END = WINDOW_GAINED_FOCUS,

	// Input events
	KEY_PRESSED,
	KEY_RELEASED,
	MOUSE_BUTTON_PRESSED,
	MOUSE_BUTTON_RELEASED,
	INPUT_END = MOUSE_BUTTON_RELEASED,

	// Physics events
	COLLISION,
	TRIGGER_ENTER,
	TRIGGER_EXIT,
	PHYSICS_END = TRIGGER_EXIT,

	// Entity events
	ENTITY_CREATED,
	ENTITY_DESTROYED,
	ENTITY_END = ENTITY_DESTROYED,

	// Custom direct events
	MESSAGE,
	CUSTOM_END = MESSAGE,

	// Custom global events
	GLOBAL_MESSAGE,
	GLOBAL_END = GLOBAL_MESSAGE,

	// Get the total number of event types
	SIZE 
};

enum class EventCategory
{
	SYSTEM_EVENT,
	INPUT_EVENT,
	PHYSICS_EVENT,
	ENTITY_EVENT,
	CUSTOM_DIRECT_EVENT,
	CUSTOM_GLOBAL_EVENT
};

struct EmptyEvent {};

struct EntityEvent
{
	EntityEvent(const std::shared_ptr<Entity>& entity_ptr);
	const std::shared_ptr<Entity> entity;
};

using EventData = std::variant<
							EmptyEvent,
							EntityEvent>;

using Actor = std::variant<std::shared_ptr<sf::RenderWindow>, std::shared_ptr<Entity>>;

class Event
{
protected:

	EventType type;
	EventData data;

public:

	Event(EventType event_type);
	Event(EventType event_type, EventData _data);
	EventCategory get_event_category() const;
	EventType get_event_type() const;
	template<typename T>

	T get_data() const
	{
		if (std::holds_alternative<T>(data))
		{
			return std::get<T>(data);
		}

		std::cerr << "Error: EventData does not hold the requested type." << std::endl;
	}
};


using EventCallback = std::function<void(const Event&)>;
using EventBus = std::array<std::unordered_map<Actor, EventCallback>, static_cast<size_t>(EventType::SIZE)>;

class EventManager
{
protected:

	static EventBus event_bus;

	EventManager();

public:

	static void suscribe(Actor self, EventType event_type, EventCallback callback);
	static void publish(const Event& event);
	static void publish(const Event& event, Actor target);
};