#include "renderer.hpp"

void Renderer::start()
{
    if (!object) 
    {
        sf::Image img;
        img.create(2, 2);

        img.setPixel(0, 0, sf::Color(255, 0, 255));
        img.setPixel(1, 0, sf::Color::Black);
        img.setPixel(0, 1, sf::Color::Black);
        img.setPixel(1, 1, sf::Color(255, 0, 255));

        sf::Texture tex;
        tex.loadFromImage(img);
        tex.setRepeated(false);

		std::shared_ptr<sf::RectangleShape> texture_error = std::make_shared<sf::RectangleShape>(sf::Vector2f(100, 100));
		object = texture_error;
	}

	EventManager::publish(Event(EventType::RENDERER_CREATED, EntityEvent(owner.lock())));
    /*
    if (auto shape = std::dynamic_pointer_cast<sf::Shape>(visual_obj)) 
    {
        shape->setFillColor(color);
    }

    else if (auto sprite = std::dynamic_pointer_cast<sf::Sprite>(visual_obj)) 
    {
        sprite->setColor(color);
    }

    else if (auto text = std::dynamic_pointer_cast<sf::Text>(visual_obj)) 
    {
        text->setFillColor(color);
    }*/
}

void Renderer::update()
{

}

uint8_t Renderer::get_layer() const
{
    return layer;
}

std::shared_ptr<sf::Drawable> Renderer::get_object() const
{
    return object;
}