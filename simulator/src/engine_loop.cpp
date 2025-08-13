#include <engine_loop.hpp>
#include <entity_test.hpp>
#include <scenario.hpp>

// Constructor
Engine::Engine()
{
    this->window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "SFML works!");
}

// Destructor
Engine::~Engine()
{
}

void Engine::update()
{
	// Poll events
    sf::Event ev;
    while (this->window->pollEvent(ev))
    {
        if (ev.type == sf::Event::Closed)
            this->window->close();
    }

    // Update

}

void Engine::render()
{
    this->window->clear();
    this->window->display();
}

// Main loop function
void Engine::run()
{
	Scenario scenario;
	std::shared_ptr<Entity> entity1 = EntityFactory<EntityTest>::create("Test1");
	scenario.add_entity(entity1);

	scenario.start();

    while (this->window->isOpen())
    {
        auto start = std::chrono::high_resolution_clock::now();
		this->update();
		this->render();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        int fps = (duration.count() > 0) ? (1000 / duration.count()) : 0;
        this->window->setTitle("SFML works! FPS: " + std::to_string(fps));
    }
}
