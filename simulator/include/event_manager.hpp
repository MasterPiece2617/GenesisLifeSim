#pragma once

#include <array>
#include <functional>
#include <memory>
#include <optional>
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

	// Input events
	KEY_PRESSED,
	KEY_RELEASED,
	MOUSE_BUTTON_PRESSED,
	MOUSE_BUTTON_RELEASED,
	MOUSE_WHEEL_SCROLLED,
	MOUSE_MOVED,

	// Physics events
	COLLISION,
	TRIGGER_ENTER,
	TRIGGER_EXIT,

	// Entity events
	RENDERER_CREATED,
	RENDERER_DESTROYED,
	ENTITY_CREATED,
	ENTITY_DESTROYED,

	// Custom direct events
	MESSAGE,

	// Custom global events
	GLOBAL_MESSAGE,
};

struct EventTypeInfo
{
	static const EventType system_end = EventType::WINDOW_GAINED_FOCUS;
	static const EventType input_end = EventType::MOUSE_MOVED;
	static const EventType physics_end = EventType::TRIGGER_EXIT;
	static const EventType entity_end = EventType::ENTITY_DESTROYED;
	static const EventType custom_end = EventType::MESSAGE;
	static const EventType custom_global_end = EventType::GLOBAL_MESSAGE;
	static const size_t size = static_cast<size_t>(custom_global_end) + 1;
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

struct MouseWheelEvent
{
	MouseWheelEvent(float _delta);
	const float delta;
};

struct MouseButtonEvent
{
	MouseButtonEvent(sf::Mouse::Button _button);
	const sf::Mouse::Button button;
};

using EventData = std::variant<
							EmptyEvent,
							MouseWheelEvent,
							MouseButtonEvent,
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
		exit(EXIT_FAILURE);
	}
};


using EventCallback = std::function<void(const Event&)>;
using EventBus = std::array<std::unordered_map<Actor, EventCallback>, EventTypeInfo::size>;

class EventManager
{
protected:

	static EventBus event_bus;

	EventManager();

public:

	static void suscribe(Actor self, EventType event_type, EventCallback callback);
	static bool desuscribe(Actor self, EventType event_type);
	static void publish(const Event& event);
	static void publish(const Event& event, Actor target);
};

class InputManager
{
protected:

	static float scroll_delta;

public:

	static void init();
	static void update();
	static float get_scroll_delta();
};