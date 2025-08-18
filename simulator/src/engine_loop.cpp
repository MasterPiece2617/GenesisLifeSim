#include <engine_loop.hpp>
#include <entity_test.hpp>
#include <scene.hpp>

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
	// código de prueba que será eliminado
	Scene scenario;
	std::shared_ptr<Entity> entity1 = EntityFactory<EntityTest>::create("Test1");
	scenario.add_entity(entity1);
	entity1->print();
	std::shared_ptr<Entity> entity2 = EntityFactory<EntityTest>::create("Hijo de Test1");

	std::shared_ptr<Entity> entity3 = EntityFactory<Entity>::create("Padre de test1");
    if (entity3->get_component<ComponentTest>() != nullptr)
        std::cout << "Si" << std::endl;
    else
    {
		std::cout << "No" << std::endl;
    }

    if (entity3->remove_component<ComponentTest>())
        std::cout << "Se ha eliminado el componente" << std::endl;
    else
		std::cout << "No se ha eliminado el componente" << std::endl;

    entity3->add_component(std::make_shared<ComponentTest>(entity3->shared_from_this()));
	scenario.add_entity(entity2);
	entity1->add_child(entity2);
	scenario.add_entity(entity3);

    if (entity3->get_component<ComponentTest>() != nullptr)
        std::cout << "Si" << std::endl;
    else
    {
        std::cout << "No" << std::endl;
    }

    if (entity3->remove_component<ComponentTest>())
        std::cout << "Se ha eliminado el componente" << std::endl;
    else
        std::cout << "No se ha eliminado el componente" << std::endl;
    if (entity3->get_component<ComponentTest>() != nullptr)
        std::cout << "Si" << std::endl;
    else
    {
        std::cout << "No" << std::endl;
    }
	entity1->set_parent(entity3);

	entity1->print_family();

	scenario.start();
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
