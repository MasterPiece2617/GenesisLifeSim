#include <engine_loop.hpp>

// Constructor
Engine::Engine()
{
    this->window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "SFML works!");

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
}

// Destructor
Engine::~Engine() {}

void Engine::update()
{
	// Poll events:

	// System events
    sf::Event ev;
    while (this->window->pollEvent(ev))
    {
        if (ev.type == sf::Event::Closed)
        {
            EventManager::publish(Event(EventType::WINDOW_CLOSED, EventData()));
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
                /*
                sf::Transformable* transformable = dynamic_cast<sf::Transformable*>(renderer->get_object().get());
                if (transformable)
                {
                    transformable->setPosition(owner->get_transform()->g);
                    transformable->setRotation(owner->get_rotation());
                    transformable->setScale(owner->get_scale());
				}

                this->window->draw();*/
            }
        }
	}

    this->window->display();
}

// Main loop function
void Engine::run()
{
	// código de prueba que será eliminado
	std::shared_ptr<Entity> entity1 = EntityFactory<EntityTest>::create("Test1");
	scene.add_entity(entity1);
    entity1->add_component(std::make_shared<EventTest1>(entity1->shared_from_this()));
	std::shared_ptr<Entity> entity2 = EntityFactory<EntityTest>::create("Hijo de Test1");
	std::shared_ptr<Entity> entity3 = EntityFactory<Entity>::create("Padre de test1");
    entity3->add_component(std::make_shared<ComponentTest>(entity3->shared_from_this()));
	scene.add_entity(entity2);
	entity1->add_child(entity2);
	scene.add_entity(entity3);
	entity1->set_parent(entity3);
	entity1->~Entity();

	// termina código de prueba que será eliminado
    while (this->window->isOpen())
    {
        auto start = std::chrono::high_resolution_clock::now();
		this->update();
		this->render();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        double fps = (duration.count() > 0) ? (1000 / duration.count()) : 0;
        this->window->setTitle("SFML works! FPS: " + std::to_string(fps));
    }
}
