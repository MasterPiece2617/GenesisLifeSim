#include <engine_loop.hpp>

// Constructor
Engine::Engine()
{
    this->window = std::make_shared<sf::RenderWindow>(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), "Genesis BioSim");
	ImGui::SFML::Init(*this->window);


    EventManager::suscribe(this->window, EventType::WINDOW_CLOSED, [&](const Event& event)
    {
        this->window->close();
	});

    EventManager::suscribe(this->window, EventType::RENDERER_CREATED, [&](const Event& event)
    {
        auto renderer = event.get_data<EntityEvent>().entity->get_component<SpriteRenderer>();

        if (renderer)
        {
            uint8_t layer = renderer->get_layer();
            //this->render_queue[layer].push_back(renderer);
		}
    });

	InputManager::init();
}

// Destructor
Engine::~Engine() {}

void Engine::update()
{
	InputManager::update();
 
	// Poll events:

	// System events
	sf::Clock deltaClock;
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
		ImGui::SFML::ProcessEvent(ev);
    }

    // Update
    for (auto& entity : scene.get_entities())
    {
        if (entity->get_is_active())
        {
            entity->update();
        }
    }

	ImGui::SFML::Update(*this->window, deltaClock.restart());
    this->window->setView(scene.get_main_camera()->get_view());
}

void Engine::render()
{
    this->window->clear();

    if(this->terrain)
    {
        this->terrain->draw(*this->window, sf::RenderStates::Default);
	}

    std::vector<std::vector<std::shared_ptr<SpriteRenderer>>> render_queue = std::vector<std::vector<std::shared_ptr<SpriteRenderer>>>(256, std::vector<std::shared_ptr<SpriteRenderer>>());

    sf::View view = scene.get_main_camera()->get_view();
    sf::FloatRect bounds(
        view.getCenter().x - view.getSize().x / 2.0f,
        view.getCenter().y - view.getSize().y / 2.0f,
        view.getSize().x,
        view.getSize().y
    );

    int chunk_size_world = Constants::px_mt * Constants::chunk_size;

    int start_x = static_cast<int>(std::floor(bounds.left / chunk_size_world)) * Constants::chunk_size;
    int end_x = static_cast<int>(std::ceil((bounds.left + bounds.width) / chunk_size_world)) * Constants::chunk_size;

    int start_y = static_cast<int>(std::floor(bounds.top / chunk_size_world)) * Constants::chunk_size;
    int end_y = static_cast<int>(std::ceil((bounds.top + bounds.height) / chunk_size_world)) * Constants::chunk_size;


    for (int y = start_y; y < end_y; y += Constants::chunk_size)
    {
        for (int x = start_x; x < end_x; x += Constants::chunk_size)
        {
            std::vector<std::shared_ptr<Entity>> _entities = scene.get_chunk_entities(sf::Vector2f(x, y));
            if (_entities.size() > 0)

            for (auto& entity : _entities)
            {
                std::shared_ptr<SpriteRenderer> renderer = entity->get_component<SpriteRenderer>();

                if (!renderer)
                {
                    continue;
                }

                if (entity->get_is_active() && renderer->get_is_active())
                {
                    render_queue[renderer->get_layer()].push_back(renderer);
                }
            }
        }
    }
    
    for (const std::vector<std::shared_ptr<SpriteRenderer>>& layer : render_queue)
    {
        sf::VertexArray final_vertices(sf::Quads);

        for (const std::shared_ptr<SpriteRenderer>& renderer : layer)
        {
            const sf::VertexArray& batch = renderer->get_batch();

            for (size_t i = 0; i < batch.getVertexCount(); ++i)
            {
                final_vertices.append(batch[i]);
            }
        }

        this->window->draw(final_vertices, &Texture::get_atlas());
	}

	ImGui::SFML::Render(*this->window);
    this->window->display();
}

// Main loop function
void Engine::run()
{
	scene.load();

    // to create terrain, provisional?
    this->texture_atlas = std::make_shared<Atlas>();
    this->terrain = std::make_shared<EntityTerrain>("resources/maps/map_2.zadat", this->texture_atlas);
	this->scene.add_entity(this->terrain);

	this->window->setView(scene.get_main_camera()->get_view());

    while (this->window->isOpen())
    {
        Time::update();
        auto start = std::chrono::high_resolution_clock::now();
        this->update();

        ImGui::Begin("Info");
		ImGui::Text("Entities: %d", scene.get_entities().size());
		ImGui::Text("FPS: %.2f", 1.0f / Time::get_delta());
        ImGui::End();

        this->render();

		
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        //double fps = (duration.count() > 0) ? (1000 / duration.count()) : 0;
        this->window->setTitle("Genesis BioSim");
    }

	ImGui::SFML::Shutdown();
}
