#include <camera.hpp>

Camera::Camera(std::string _name) : Entity(_name) 
{
	view = sf::View(sf::FloatRect(0, 0, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT));
}

void Camera::init()
{
	add_component(std::make_shared<CameraController>(shared_from_this()));
}

void Camera::set_zoom(float new_zoom)
{
	view.zoom(new_zoom);
	zoom *= new_zoom;
}

float Camera::get_zoom() const
{
	return zoom;
}

sf::View& Camera::get_view()
{
	return view;
}

CameraController::CameraController(std::weak_ptr<Entity> _owner, float _speed) : Component(_owner), speed(_speed){}

void CameraController::update()
{
	std::shared_ptr<Camera> camera_ptr = std::dynamic_pointer_cast<Camera>(owner.lock());

	if (camera_ptr) 
	{
		camera_ptr->set_zoom(1 - (InputManager::get_scroll_delta() / 10));

		camera_ptr->get_transform().translate(sf::Vector2f(
			(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A)) * speed * Time::get_delta() * camera_ptr->get_zoom() * 10,
			(sf::Keyboard::isKeyPressed(sf::Keyboard::S) - sf::Keyboard::isKeyPressed(sf::Keyboard::W)) * speed * Time::get_delta() * camera_ptr->get_zoom() * 10
		));

		camera_ptr->get_view().setCenter(owner.lock()->get_transform().get_position());
	}
}