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

    EventManager::suscribe(this->window, EventType::MOUSE_BUTTON_PRESSED, [&](const Event& event)
        {
            sf::Mouse::Button button = event.get_data<MouseButtonEvent>().button;

            switch (button)
            {
            case sf::Mouse::Button::Left:

                sf::Vector2f mouse_world_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

                for (auto& entity : Scene::instance().get_chunk_entities(mouse_world_pos))
                {
                    std::shared_ptr<SpriteRenderer> renderer = entity->get_component<SpriteRenderer>();
                    std::cout << entity->get_name() << std::endl;

                    if (!renderer)
                    {
                        continue;
                    }

                    sf::VertexArray& arr = renderer->get_batch();

                    for (std::size_t i = 0; i + 3 < arr.getVertexCount(); i += 4)
                    {
                        const sf::Vector2f& p0 = arr[i + 0].position;
                        const sf::Vector2f& p2 = arr[i + 2].position;

                        float min_x = std::min(p0.x, p2.x);
                        float max_x = std::max(p0.x, p2.x);
                        float min_y = std::min(p0.y, p2.y);
                        float max_y = std::max(p0.y, p2.y);

                        if (mouse_world_pos.x * Constants::px_mt >= min_x && mouse_world_pos.x * Constants::px_mt <= max_x &&
                            mouse_world_pos.y * Constants::px_mt >= min_y && mouse_world_pos.y * Constants::px_mt <= max_y)
                        {
                            selected_entity = entity;
                            break;
                        }
                    }
                }
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
        case sf::Event::MouseButtonPressed:
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                MouseButtonEvent data(sf::Mouse::Button::Left);
                Event ev(EventType::MOUSE_BUTTON_PRESSED, data);
                EventManager::publish(ev);
            }
            break;
        }

        ImGui::SFML::ProcessEvent(ev);
    }

    // Update
    if (caract_load)
    {
    
    for (auto& entity : Scene::instance().get_entities())
    {
        if (entity->get_is_active())
        {
            entity->get_transform().reset_dirty();

            sf::Vector2f pos = entity->get_transform().get_position();
            sf::Vector2f scale = entity->get_transform().get_scale();

            entity->update();

            if (entity->get_transform().get_dirty())
            {
                Scene::instance().update_entity_grid(entity, pos, scale);
            }
        }
    }
    }

    ImGui::SFML::Update(*this->window, deltaClock.restart());
    this->window->setView(Scene::instance().get_main_camera()->get_view());
}

void Engine::render()
{
    this->window->clear();

    
    std::vector<std::vector<std::shared_ptr<SpriteRenderer>>> render_queue = std::vector<std::vector<std::shared_ptr<SpriteRenderer>>>(256, std::vector<std::shared_ptr<SpriteRenderer>>());

    sf::View view = Scene::instance().get_main_camera()->get_view();
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

    std::unordered_set<std::shared_ptr<Entity>> visited_entities;

    if (caract_load)
    {

    for (int y = start_y; y < end_y; y += Constants::chunk_size)
    {
        for (int x = start_x; x < end_x; x += Constants::chunk_size)
        {
            std::vector<std::shared_ptr<Entity>> _entities = Scene::instance().get_chunk_entities(sf::Vector2f(x, y));

            for (auto& entity : _entities)
            {
                if (visited_entities.count(entity) > 0)
                {
                    continue;
                }

                visited_entities.insert(entity);

                std::shared_ptr<SpriteRenderer> renderer = entity->get_component<SpriteRenderer>();

                if (!renderer)
                {
                    continue;
                }

                if (entity->get_is_active() && renderer->get_is_active())
                {
                    if (entity->get_transform().get_dirty())
                    {
                        renderer->build_batch();
                    }

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

    }

    ImGui::SFML::Render(*this->window);
    this->window->display();
}

// Main loop function
void Engine::run()
{
    Scene::instance().load(organism_config);
    this->window->setView(Scene::instance().get_main_camera()->get_view());

    float fps_accumulator = 0.0f;
    float fps_display = 0.0f;
    float fps_timer = 0.0f;


    while (this->window->isOpen())
    {
        Time::update();
        this->update();

        float delta = Time::get_delta();
        fps_timer += delta;

        if (fps_timer >= 1.0f)
        {
            fps_display = 1.0f / delta;
            fps_timer = 0.0f;
        }

        if (!caract_load)
        {
            ImGui::Begin("Seleccione las caracteristicas:");
            ImGui::Text("Seleccione las caracteristicas que desea cargar en la simulacion.");
            ImGui::Separator();

            // --- NUEVO CÓDIGO PARA MODIFICAR STATS ---

            ImGui::Text("Stats del Organismo:");
            // Conecta el SliderInt a g_Config.vision
            ImGui::SliderInt("Vision", &organism_config.vision_radius, 1, 20);
         
            ImGui::Separator();
            ImGui::Text("Stats de Comportamiento:");
            // Conecta el SliderFloat a g_Config.speed
            ImGui::SliderFloat("Velocidad (Speed)", &organism_config.move_speed, 1.0f, 15.0f);

            // --- FIN DEL NUEVO CÓDIGO ---

            ImGui::Separator();

            // El ImGui::Selectable() que tenías no hacía nada útil aquí.

            if (ImGui::Button("Cargar Caracteristicas"))
            {
                caract_load = true;

                // ¡IMPORTANTE! 
                // Ahora, tu función que crea los organismos debe usar g_Config
                // Ejemplo:
                Scene::instance().load(organism_config); 
            }

            ImGui::End();

        }
        else {

            sf::Vector2f mouse_world_pos = window->mapPixelToCoords(sf::Mouse::getPosition());
            mouse_world_pos.x /= Constants::px_mt;
            mouse_world_pos.y /= Constants::px_mt;

            ImGui::Begin("Info");
            ImGui::Text("Entities: %d", Scene::instance().get_entities().size());
            ImGui::Text("FPS: %.2f", fps_display);
            ImGui::Text("Zoom: %.2f", Scene::instance().get_main_camera()->get_zoom());
            ImGui::Text("Mouse x: %f y: %f", mouse_world_pos.x, mouse_world_pos.y);

            if (selected_entity)
            {
                ImGui::Text("%s", selected_entity->get_name().c_str());
            }

            ImGui::End();
        }

        this->render();
    }

    ImGui::SFML::Shutdown();
}