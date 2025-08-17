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
	void init_transform();
	virtual void init();
	const std::string& get_name() const;
	void set_name(const std::string& _name);
	bool add_component(std::shared_ptr<Component> component);
	virtual void print() const;
	void print_family() const;
	bool get_is_active() const;
	void deactivate();
	void activate();

	Transform& get_transform() const;
	bool add_tag(const std::string& tag);
	bool has_tag(const std::string& tag) const;
	bool remove_tag(const std::string& tag);
	bool set_parent(std::shared_ptr<Entity> _parent);
	std::shared_ptr<Entity> get_parent() const;
	std::vector<std::shared_ptr<Entity>> get_children() const;
	bool add_child(std::shared_ptr<Entity> child);
	bool remove_child(std::shared_ptr<Entity> child);

	void start();
	void update();
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
		entity->init_tranform();
		entity->init();
		return entity;
	}
};