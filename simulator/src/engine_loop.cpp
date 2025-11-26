#include <engine_loop.hpp>

// Constructor
Engine::Engine()
{
    this->window = std::make_shared<sf::RenderWindow>(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), "Genesis BioSim");

    // Create the global texture atlas at startup
    //this->texture_atlas = std::make_shared<Atlas>();
    //Texture::set_atlas(this->texture_atlas);

    ImGui::SFML::Init(*this->window);

    EventManager::suscribe(this->window, EventType::WINDOW_CLOSED, [&](const Event& event)
        {
            this->window->close();
        });

    EventManager::suscribe(this->window, EventType::MOUSE_BUTTON_PRESSED, [&](const Event& event)
        {
            if (ImGui::GetIO().WantCaptureMouse) return;

            sf::Mouse::Button button = event.get_data<MouseButtonEvent>().button;

            switch (button)
            {
            case sf::Mouse::Button::Left:
            {
                sf::Vector2f mouse_world_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

                if (this->placement_mode)
                {
                    if (this->terrain)
                    {
                        sf::Vector2f mouse_meters = mouse_world_pos / (float)Constants::px_mt;

                        if (mouse_meters.x >= 0 && mouse_meters.x < this->terrain->get_width() &&
                            mouse_meters.y >= 0 && mouse_meters.y < this->terrain->get_height())
                        {
                            if (this->terrain->walkable((uint16_t)mouse_meters.x, (uint16_t)mouse_meters.y))
                            {
                                // spawn!!!!!!!!!
                                Scene::instance().spawn_organisms(mouse_meters, this->organism_config);
                            }
                            else
                            {
                                std::cout << "No se puede colocar aqui (Terreno no caminable)" << std::endl;
                            }
                        }
                    }

                    //this->placement_mode = true;
                }
                else
                {
                    // Selection mode
                    for (auto& entity : Scene::instance().get_chunk_entities(mouse_world_pos / (float)Constants::px_mt))
                    {
                        auto organism = std::dynamic_pointer_cast<Organism>(entity);
                        if (!organism)
                        {
                            continue;
                        }

                        sf::Vector2f org_pos = organism->get_transform().get_position();
                        if (mouse_world_pos.x / Constants::px_mt >= org_pos.x - 0.5f && mouse_world_pos.x / Constants::px_mt <= org_pos.x + 1 &&
                            mouse_world_pos.y / Constants::px_mt >= org_pos.y - 0.5f && mouse_world_pos.y / Constants::px_mt <= org_pos.y + 1)
                        {
                            selected_entity = organism;
                            break;
                        } 
                        else
                        {
                            selected_entity = nullptr;
                        }
                    }
                }
            }
            break;

            case sf::Mouse::Button::Right:
            {
                if (this->placement_mode)
                {
                    this->placement_mode = false;
                } 
                else
                {
                    selected_entity = nullptr;
                }  

            }
            break;
            }
        });

    InputManager::init();

    PopulationStats::init(this->window);
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
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                MouseButtonEvent data(sf::Mouse::Button::Right);
                Event ev(EventType::MOUSE_BUTTON_PRESSED, data);
                EventManager::publish(ev);
            }
            break;
        }

        ImGui::SFML::ProcessEvent(ev);
    }
    //ImGui::SFML::ProcessEvent(ev);
  

    // Update  
  
    for (auto& entity : Scene::instance().get_entities())
    {
        if (entity->get_is_active())
        {
            sf::Vector2f pos = entity->get_transform().get_position();
            sf::Vector2f scale = entity->get_transform().get_scale();

            entity->update();

            if (entity->get_transform().get_dirty())
            {
                Scene::instance().update_entity_grid(entity, pos, scale);
            }
        }
    }

    ImGui::SFML::Update(*this->window, deltaClock.restart());
    this->window->setView(Scene::instance().get_main_camera()->get_view());
}

void Engine::render()
{
  this->window->clear();
 
  if (terrain)
  {
      terrain->draw(*window, sf::RenderStates::Default);
  }

  std::vector<std::vector<std::shared_ptr<SpriteRenderer>>> render_queue =
      std::vector<std::vector<std::shared_ptr<SpriteRenderer>>>(
          256, std::vector<std::shared_ptr<SpriteRenderer>>());
  
  sf::View view = Scene::instance().get_main_camera()->get_view();
  sf::FloatRect bounds(view.getCenter().x - view.getSize().x / 2.0f,
                       view.getCenter().y - view.getSize().y / 2.0f,
                       view.getSize().x, view.getSize().y);

  int chunk_size_world = Constants::px_mt * Constants::chunk_size;

  // Correct calculation for chunk coordinates from world coordinates
  int start_x = static_cast<int>(std::floor(bounds.left / chunk_size_world));
  int end_x = static_cast<int>(std::ceil((bounds.left + bounds.width) / chunk_size_world));

  int start_y = static_cast<int>(std::floor(bounds.top / chunk_size_world));
  int end_y = static_cast<int>(std::ceil((bounds.top + bounds.height) / chunk_size_world));

    std::unordered_set<std::shared_ptr<Entity>> visited_entities;

	//if (caract_load)
	
		for (int y = start_y; y <= end_y; ++y)
		{
			for (int x = start_x; x <= end_x; ++x)
			{
				std::vector<std::shared_ptr<Entity>> _entities = Scene::instance().get_chunk_entities(sf::Vector2f(x * Constants::chunk_size, y * Constants::chunk_size));
				for (auto& entity : _entities)
				{
					if (visited_entities.count(entity) > 0)
					{
						continue;
					}
					visited_entities.insert(entity);

					std::shared_ptr<SpriteRenderer> renderer = entity->get_component<SpriteRenderer>();

					if (renderer && entity->get_is_active() && renderer->get_is_active())
					{
						if (entity->get_transform().get_dirty())
						{
							renderer->build_batch();
							entity->get_transform().reset_dirty();
						}
						render_queue[renderer->get_layer()].push_back(renderer);
					}
				}
			}
		}
	//}

	for (const auto& layer : render_queue)
	{
		sf::VertexArray final_vertices(sf::Quads);
		for (const auto& renderer : layer)
		{
			const sf::VertexArray& batch = renderer->get_batch();
			for (size_t i = 0; i < batch.getVertexCount(); ++i)
			{
				final_vertices.append(batch[i]);
			}
		}
		this->window->draw(final_vertices, &Texture::get_atlas()); 
	}


    static float time_accumulator = 0.0f;

    time_accumulator += Time::get_delta() * Time::get_simulation_speed();
    if (time_accumulator >= 0.2f)
    {
        PopulationStats::update_history(time_accumulator);
        time_accumulator = 0.0f;
    }

  ImGui::SFML::Render(*this->window);
  this->window->display();
}

// Main loop function
void Engine::run() 
{
  float fps_accumulator = 0.0f;
  float fps_display = 0.0f;
  float fps_timer = 0.0f;
  std::string selected_map = "";

  ImGui::CreateContext();
  ImPlot::CreateContext();

  this->window->setView(scene.get_main_camera()->get_view());

  while (this->window->isOpen()) {
    Time::update();
    //auto start = std::chrono::high_resolution_clock::now();
    this->update();

    float delta = Time::get_delta();
    fps_timer += delta;
    if (fps_timer >= 1.0f)
    {
        fps_display = 1.0f / delta;
        fps_timer = 0.0f;
    }

    if (!map_loaded)
    {

      ImGuiMenu::show_select_map_window(this->map_loaded, selected_map, 
                                        EntityTerrain::get_map_files("resources/maps", ".zadat"),
                                        this->terrain, this->texture_atlas);

      if (this->terrain)
      {
        auto main_camera = Scene::instance().get_main_camera();

        if (main_camera && !centered)
        {
            
            main_camera->get_transform().set_position({ (this->terrain->get_width() * Constants::px_mt) / 2.0f, (this->terrain->get_height() * Constants::px_mt) / 2.0f });
            main_camera->get_view().setCenter(main_camera->get_transform().get_position().x, main_camera->get_transform().get_position().y);
            this->window->setView(main_camera->get_view());
            main_camera->set_zoom(1.0f);
            centered = true;
            std::cout << "Camera centered on terrain." << std::endl;
        }

        Scene::instance().load(this->organism_config);

      }
    }
    else 
    {
        if (this->terrain) 
        {
          Debugger::imgui_terrain(this->terrain, this->window);
        }

        sf::Vector2f mouse_world_pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
        mouse_world_pos.x;
        mouse_world_pos.y;

        Debugger::imgui_scene(fps_display, mouse_world_pos);
  
        ImPlotMenu::show_menu(this->organism_config, this->window.get(), terrain, this->map_loaded, selected_map, this->centered, this->placement_mode);
        
        std::shared_ptr<Organism> selected_organism = std::dynamic_pointer_cast<Organism>(selected_entity);
        Debugger::show_selected_entity(selected_organism);
        
    }
      
      this->render();
      //this->window->setTitle("Genesis BioSim");  
  }
  
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  ImGui::SFML::Shutdown();
}

void Engine::reset_simulation(std::shared_ptr<EntityTerrain>& terrain, bool& map_loaded, std::string& selected_map, bool& centered)
{
    Scene::instance().clear();

    PopulationStats::reset();

    terrain.reset();
    map_loaded = false;
    centered = false;
    selected_map = "";
 
    auto main_camera = Scene::instance().get_main_camera();
    if (main_camera)
    {
        main_camera->get_transform().set_position({ Config::WINDOW_WIDTH / (2.0f * Constants::px_mt), Config::WINDOW_HEIGHT / (2.0f * Constants::px_mt) });
        main_camera->set_zoom(1.0f);
    }

    std::cout << "Simulation reset completed." << std::endl;
}
