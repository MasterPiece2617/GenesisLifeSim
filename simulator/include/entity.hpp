#pragma once

#include <iostream>
#include <vector>

#include <transform.hpp>

class Component; // Forward declaration


class Entity : public std::enable_shared_from_this<Entity>
{
protected:

	std::string name;
	std::vector<std::shared_ptr<Component>> components;
	std::vector<std::string> tags;
	std::shared_ptr<Transform> transform;
	bool is_active = true;

public:
	
	Entity(const std::string& _name);

	template<typename T>
	void init(std::shared_ptr<T> self)
	{
		if (transform)
		{
			std::cerr << "Warning: Transform already initialized for entity: " << name << std::endl;
			return;
		}

		std::shared_ptr<Transform> _transform = std::make_shared<Transform>(self);
		transform = _transform;
		components.push_back(_transform);
	}

	const std::string& get_name() const;
	void set_name(const std::string& _name);
	bool add_component(std::shared_ptr<Component> component);
	virtual void print() const;
	bool get_is_active() const;
	void deactivate();
	void activate();

	Transform& get_transform() const;
	bool add_tag(const std::string& tag);
	bool has_tag(const std::string& tag) const;
	bool remove_tag(const std::string& tag);
	bool set_parent(std::shared_ptr<Entity> _parent);
	std::shared_ptr<Entity> get_parent() const;
	bool add_child(std::shared_ptr<Entity> child);
	bool remove_child(std::shared_ptr<Entity> child);

	virtual void start();
	virtual void update();
	virtual ~Entity() = default;
};

template<typename T>
class EntityFactory
{
public:

	static std::shared_ptr<T> create(const std::string& name)
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must inherit from Entity");
		std::shared_ptr<T> entity = std::make_shared<T>(name);
		entity->init(entity);
		return entity;
	}
};