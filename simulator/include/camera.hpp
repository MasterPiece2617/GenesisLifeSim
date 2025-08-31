#pragma once 

#include <SFML/Graphics.hpp>

#include <component.hpp>
#include <config.hpp>
#include <entity.hpp>
#include <event_manager.hpp>
#include <utils.hpp>

class Camera : public Entity
{
protected:

	sf::View view;
	float zoom = 1;

public:

	Camera(std::string _name);
	void init() override;
	void set_zoom(float new_zoom);
	float get_zoom() const;
	sf::View& get_view();
	~Camera() override = default;
};

class CameraController : public Component
{
protected:

	float speed;

public:

	CameraController(std::weak_ptr<Entity> _owner, float _speed = 200);
	//void start() override;
	void update() override;
	~CameraController() override = default;
};