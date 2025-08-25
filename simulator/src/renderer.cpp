#include "renderer.hpp"

std::shared_ptr<sf::Texture> Renderer::error_texture = nullptr;

void Renderer::set_error_texture()
{
    sf::Image img;
    img.create(2, 2);
    img.setPixel(0, 0, sf::Color(255, 0, 255));
    img.setPixel(1, 0, sf::Color::Black);
    img.setPixel(0, 1, sf::Color::Black);
    img.setPixel(1, 1, sf::Color(255, 0, 255));

    error_texture = std::make_shared<sf::Texture>();
    error_texture->loadFromImage(img);
    error_texture->setRepeated(false);
}

Renderer::Renderer(std::weak_ptr<Entity> _owner, std::shared_ptr<sf::Drawable> _object, std::shared_ptr<sf::Texture> _texture, uint8_t _layer) : Component(_owner), object(_object), texture(_texture), layer(_layer) {}

void Renderer::start()
{
    if (!object) 
    {
        if (!error_texture)
        {
            set_error_texture();
        }
			
		texture = error_texture;

		std::shared_ptr<sf::RectangleShape> texture_error = std::make_shared<sf::RectangleShape>(sf::Vector2f(100, 100));
        texture_error->setTexture(texture.get());
		object = texture_error;

        std::cerr << "Warning: No drawable object provided to Renderer component of entity '" << owner.lock()->get_name() << "'. Using default placeholder object." << std::endl;
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

void Renderer::change_texture(std::shared_ptr<sf::Texture> new_texture)
{
    texture = new_texture;

    if (auto shape = std::dynamic_pointer_cast<sf::Shape>(object)) 
    {
        shape->setTexture(texture.get());
    }
    else if (auto sprite = std::dynamic_pointer_cast<sf::Sprite>(object)) 
    {
        sprite->setTexture(*texture);
	}
}
