#include <engine_loop.hpp>

// Constructor
Engine::Engine() {
  this->window = std::make_shared<sf::RenderWindow>(
      sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT),
      "Genesis BioSim");
  ImGui::SFML::Init(*this->window);

  EventManager::suscribe(this->window, EventType::WINDOW_CLOSED,
                         [&](const Event &event) { this->window->close(); });

  EventManager::suscribe(
      this->window, EventType::RENDERER_CREATED, [&](const Event &event) {
        auto renderer = event.get_data<EntityEvent>()
                            .entity->get_component<SpriteRenderer>();

        if (renderer) {
          uint8_t layer = renderer->get_layer();
          // this->render_queue[layer].push_back(renderer);
        }
      });

  InputManager::init();
}

// Destructor
Engine::~Engine() {}

void Engine::update() {
  InputManager::update();

  // Poll events:

  // System events
  sf::Clock deltaClock;
  sf::Event ev;
  while (this->window->pollEvent(ev)) {
    switch (ev.type) {
    case sf::Event::Closed:
      EventManager::publish(Event(EventType::WINDOW_CLOSED, EventData()));
      break;
    case sf::Event::MouseWheelScrolled:
      EventManager::publish(
          Event(EventType::MOUSE_WHEEL_SCROLLED,
                EventData(MouseWheelEvent(ev.mouseWheelScroll.delta))));
      break;
    }
    ImGui::SFML::ProcessEvent(ev);
  }

  // Update
  for (auto &entity : scene.get_entities()) {
    if (entity->get_is_active()) {
      entity->update();
    }
  }

  ImGui::SFML::Update(*this->window, deltaClock.restart());
  this->window->setView(scene.get_main_camera()->get_view());
}

void Engine::render() {
  this->window->clear();

  if (this->terrain) {
    this->terrain->draw(*this->window, sf::RenderStates::Default);
  }

  std::vector<std::vector<std::shared_ptr<SpriteRenderer>>> render_queue =
      std::vector<std::vector<std::shared_ptr<SpriteRenderer>>>(
          256, std::vector<std::shared_ptr<SpriteRenderer>>());

  sf::View view = scene.get_main_camera()->get_view();
  sf::FloatRect bounds(view.getCenter().x - view.getSize().x / 2.0f,
                       view.getCenter().y - view.getSize().y / 2.0f,
                       view.getSize().x, view.getSize().y);

  int chunk_size_world = Constants::px_mt * Constants::chunk_size;

  int start_x = static_cast<int>(std::floor(bounds.left / chunk_size_world)) *
                Constants::chunk_size;
  int end_x = static_cast<int>(
                  std::ceil((bounds.left + bounds.width) / chunk_size_world)) *
              Constants::chunk_size;

  int start_y = static_cast<int>(std::floor(bounds.top / chunk_size_world)) *
                Constants::chunk_size;
  int end_y = static_cast<int>(
                  std::ceil((bounds.top + bounds.height) / chunk_size_world)) *
              Constants::chunk_size;

  for (int y = start_y; y < end_y; y += Constants::chunk_size) {
    for (int x = start_x; x < end_x; x += Constants::chunk_size) {
      std::vector<std::shared_ptr<Entity>> _entities =
          scene.get_chunk_entities(sf::Vector2f(x, y));
      if (_entities.size() > 0)

        for (auto &entity : _entities) {
          std::shared_ptr<SpriteRenderer> renderer =
              entity->get_component<SpriteRenderer>();

          if (!renderer) {
            continue;
          }

          if (entity->get_is_active() && renderer->get_is_active()) {
            render_queue[renderer->get_layer()].push_back(renderer);
          }
        }
    }
  }

  for (const std::vector<std::shared_ptr<SpriteRenderer>> &layer :
       render_queue) {
    sf::VertexArray final_vertices(sf::Quads);

    for (const std::shared_ptr<SpriteRenderer> &renderer : layer) {
      const sf::VertexArray &batch = renderer->get_batch();

      for (size_t i = 0; i < batch.getVertexCount(); ++i) {
        final_vertices.append(batch[i]);
      }
    }

    this->window->draw(final_vertices, &Texture::get_atlas());
  }

  // debug celldata with imgui
  if (this->terrain) 
  {
    Debugger::imgui_terrain(this->terrain, this->window);
  }

  ImGui::SFML::Render(*this->window);
  this->window->display();
}

// Main loop function
void Engine::run() {
  scene.load();

  ImGui::CreateContext();
  ImPlot::CreateContext();

  static std::string selected_map = "";

  this->window->setView(scene.get_main_camera()->get_view());

  while (this->window->isOpen()) {
    Time::update();
    auto start = std::chrono::high_resolution_clock::now();
    this->update();

    // cargador de mapa
    if (!map_loaded)
    {
      ImGui::Begin("Selccionar mapa");
  
      std::vector<std::string> map_files = this->terrain->get_map_files("resources/maps", ".zadat");
  
      ImGui::Text("Mapas disponibles:");
      ImGui::Separator();
  
      for (const auto& map_file : map_files)
      {
          if (ImGui::Selectable(map_file.c_str(), selected_map == map_file))
          {
              selected_map = map_file;
          }
      }
  
      ImGui::Separator();
      ImGui::Text("Mapa seleccionado: %s", selected_map.c_str());
  
      if (ImGui::Button("Cargar mapa") && !selected_map.empty())
      {
          this->texture_atlas = std::make_shared<Atlas>();
          this->terrain = std::make_shared<EntityTerrain>(selected_map, this->texture_atlas);
          this->scene.add_entity(this->terrain);
          map_loaded = true; // To close the map selection window
          ImGui::CloseCurrentPopup();
      } 
      ImGui::End();

    } else {

      // debug scene with imgui
      Debugger::imgui_scene(this->scene);
  
      ImGui::Begin("Ejemplo implot");
      if (ImPlot::BeginPlot("Mi primer plot")) 
      {
        static float x_data[1000];
        static float y_data[1000];
        for (int i = 0; i < 1000; i++) {
          x_data[i] = i * 0.01f;
          y_data[i] = std::sin(x_data[i]);
        }
        ImPlot::PlotLine("Seno", x_data, y_data, 1000);
        ImPlot::EndPlot();
      }
      ImGui::End();

    }

    this->render();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    // double fps = (duration.count() > 0) ? (1000 / duration.count()) : 0;
    this->window->setTitle("Genesis BioSim");
  }

  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  ImGui::SFML::Shutdown();
}
