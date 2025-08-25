#include <engine_loop.hpp>

// Constructor
Engine::Engine()
{
    this->window = std::make_shared<sf::RenderWindow>(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), "Genesis BioSim");

    EventManager::suscribe(this->window, EventType::WINDOW_CLOSED, [&](const Event& event)
    {
        this->window->close();
	});

    EventManager::suscribe(this->window, EventType::RENDERER_CREATED, [&](const Event& event)
    {
        auto renderer = event.get_data<EntityEvent>().entity->get_component<Renderer>();

        if (renderer)
        {
            uint8_t layer = renderer->get_layer();
            this->render_queue[layer].push_back(renderer);
		}
    });

	InputManager::init();
}

// Destructor
Engine::~Engine() {}

void Engine::update()
{
	InputManager::update();
    /*
    std::shared_ptr<Entity> entity1 = EntityFactory<Entity>::create("Bobby");

    sf::CircleShape circle(50);
    circle.setFillColor(sf::Color::Blue);

    entity1->add_component(std::make_shared<Renderer>(entity1, std::make_shared<sf::CircleShape>(circle)));
    scene.add_entity(entity1);
*/
	// Poll events:

	// System events
    sf::Event ev;
    while (this->window->pollEvent(ev))
    {
        switch (ev.type)
        {
            case sf::Event::Closed:
                EventManager::publish(Event(EventType::WINDOW_CLOSED, EventData()));
                break;
			case sf::Event::MouseWheelScrolled:
                EventManager::publish(Event(EventType::MOUSE_WHEEL_SCROLLED, EventData(MouseWheelEvent(ev.mouseWheelScroll.delta))));
				break;
        }
    }

    // Update
    for (auto& entity : scene.get_entities())
    {
        if (entity->get_is_active())
        {
            entity->update();
        }
    }

    this->window->setView(scene.get_main_camera()->get_view());
}

void Engine::render()
{
    this->window->clear();

    for (const std::vector<std::shared_ptr<Renderer>>& layer : this->render_queue)
    {
        for (const std::shared_ptr<Renderer>& renderer : layer)
        {
			std::shared_ptr<Entity> owner = renderer->get_owner().lock();

            if (owner->get_is_active() && renderer->get_is_active())
            {
                sf::Transformable* transformable = dynamic_cast<sf::Transformable*>(renderer->get_object().get());
                if (transformable)
                {
                    transformable->setPosition(owner->get_transform().get_position());
                    transformable->setRotation(owner->get_transform().get_rotation());
                    transformable->setScale(owner->get_transform().get_scale());
				}

                this->window->draw(*renderer->get_object());
            }
        }
	}

    this->window->display();
}

// Main loop function
void Engine::run()
{
	scene.load();
	this->window->setView(scene.get_main_camera()->get_view());

    while (this->window->isOpen())
    {
        Time::update();
        auto start = std::chrono::high_resolution_clock::now();
        this->update();
        this->render();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        double fps = (duration.count() > 0) ? (1000 / duration.count()) : 0;
        this->window->setTitle("Genesis BioSim: " + std::to_string(fps) + " " + std::to_string(scene.get_entities().size()));
    }
}
