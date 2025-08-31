#include "renderer.hpp"

SpriteRenderer::SpriteRenderer(std::weak_ptr<Entity> _owner, std::string _texture_id, uint8_t _layer) : Component(_owner), layer(_layer) 
{
    set_texture(_texture_id);
    build_batch();
}

void SpriteRenderer::start()
{
   
	EventManager::publish(Event(EventType::RENDERER_CREATED, EntityEvent(owner.lock())));
    
}

void SpriteRenderer::update()
{

}

uint8_t SpriteRenderer::get_layer() const
{
    return layer;
}


void SpriteRenderer::set_texture(std::string _texture_id)
{
    texture_coords = Texture::get_region(_texture_id);
    texture_id = _texture_id;

    if (texture_coords.left == 0 && texture_coords.top == 0)
    {
        texture_id = "error_texture";

        std::cerr << "Error: texture " + _texture_id + " not found";
    }
}

void SpriteRenderer::build_batch()
{
    vertices = sf::VertexArray(sf::Quads);
    vertices.resize(4);

    const Transform& transform = owner.lock()->get_transform();
    sf::Vector2f position = transform.get_position();
    position.x *= Constants::px_mt;
    position.y *= Constants::px_mt;
    sf::Vector2f scale = transform.get_scale();
    float angle = transform.get_rotation();

    int tx = texture_coords.left;
    int ty = texture_coords.top;
    int tw = texture_coords.width;
    int th = texture_coords.height;

    sf::Vector2f local[4] = {
        { -Constants::px_mt / 2, -Constants::px_mt / 2},
        { Constants::px_mt / 2, -Constants::px_mt / 2},
        { Constants::px_mt / 2, Constants::px_mt / 2 },
        { -Constants::px_mt / 2, Constants::px_mt / 2 }
    };

    sf::Transform t;
    t.translate(position);
    t.rotate(angle);
    t.scale(scale);

    for (int i = 0; i < 4; ++i)
    {
        sf::Vector2f world_pos = t.transformPoint(local[i]);

        sf::Vector2f tex_coord;

        switch (i)
        {
            case 0: tex_coord = { static_cast<float>(tx), static_cast<float>(ty) }; 
                  break;
            case 1: tex_coord = { static_cast<float>(tx + tw), static_cast<float>(ty) }; 
                  break;
            case 2: tex_coord = { static_cast<float>(tx + tw), static_cast<float>(ty + th) };
                  break;
            case 3: tex_coord = { static_cast<float>(tx), static_cast<float>(ty + th) }; 
                  break;
        }

        vertices[i] = sf::Vertex(world_pos, tex_coord);
    }
}

sf::VertexArray& SpriteRenderer::get_batch()
{
    return vertices;
}
