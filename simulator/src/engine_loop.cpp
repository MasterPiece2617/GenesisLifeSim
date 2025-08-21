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
	entity2->set_parent(entity3);

	entity1->print_family();
	entity2->print_family();

    entity2->set_parent(entity2);
	entity2->print_family();


	// Test de metodos de transform    
    std::cout << "\n--- TESTS DE TRANSFORM ---\n";
    auto test_Entity = EntityFactory<EntityTest>::create("EntidadTransform");
    auto transform = &test_Entity->get_transform();

    // Test set/get position
    sf::Vector2f pos(10.f, 20.f);
    transform->set_position(pos);
    std::cout << "Posicion: (" << transform->get_position().x << ", " << transform->get_position().y << ")\n";

    // Test set/get scale
    sf::Vector2f scale(2.f, 3.f);
    transform->set_scale(scale);
    std::cout << "Escala: (" << transform->get_scale().x << ", " << transform->get_scale().y << ")\n";

    // Test set/get rotation
    float angle = 45.f;
    transform->set_rotation(angle);
    std::cout << "Rotacion: " << transform->get_rotation() << "\n";

    // Test local position/scale/rotation
    sf::Vector2f localPos(5.f, 5.f);
    transform->set_local_position(localPos);
    std::cout << "Posicion local: (" << transform->get_local_position().x << ", " << transform->get_local_position().y << ")\n";

    sf::Vector2f localScale(1.5f, 1.5f);
    transform->set_local_scale(localScale);
    std::cout << "Escala local: (" << transform->get_local_scale().x << ", " << transform->get_local_scale().y << ")\n";

    float localAngle = 30.f;
    transform->set_local_rotation(localAngle);
    std::cout << "Rotacion local: " << transform->get_local_rotation() << "\n";

    // Test translate_mod
    sf::Vector2f offset(3.f, 4.f);
    transform->translate_mod(offset);
    std::cout << "Posicion tras translate_mod: (" << transform->get_position().x << ", " << transform->get_position().y << ")\n";

    // Test rotate_mod
    float rotOffset = 15.f;
    transform->rotate_mod(rotOffset);
    std::cout << "Rotación tras rotate_mod: " << transform->get_rotation() << "\n";

    // Test scale_mod
    sf::Vector2f scaleOffset(0.5f, 0.5f);
    transform->scale_mod(scaleOffset);
    std::cout << "Escala tras scale_mod: (" << transform->get_scale().x << ", " << transform->get_scale().y << ")\n";

    // Test get_forward
    sf::Vector2f forward = transform->get_forward();
    std::cout << "Vector forward: (" << forward.x << ", " << forward.y << ")\n";

    // Test look_at
    sf::Vector2f target(100.f, 100.f);
    transform->look_at(target);
    std::cout << "Rotacion tras look_at: " << transform->get_rotation() << "\n";


    // Test parent/child
    auto parentEntity = EntityFactory<EntityTest>::create("Padre");
    // auto parentTransform = std::make_shared<Transform>(parentEntity);
	auto parentTransform = &parentEntity->get_transform();
    if (transform->set_parent(parentEntity))
        std::cout << "Padre asignado correctamente\n";
    else
        std::cout << "Error al asignar padre\n";

    if (transform->get_parent() == parentEntity)
        std::cout << "get_parent correcto\n";
    else
        std::cout << "get_parent incorrecto\n";

    auto childEntity = EntityFactory<EntityTest>::create("Hijo");
	auto childTransform = &childEntity->get_transform();
    if (transform->add_child(childEntity))
        std::cout << "Hijo agregado correctamente\n";
    else
        std::cout << "Error al agregar hijo\n";

    auto children = transform->get_children();
    std::cout << "Cantidad de hijos: " << children.size() << "\n";

    if (transform->remove_child(childEntity))
        std::cout << "Hijo eliminado correctamente\n";
    else
        std::cout << "Error al eliminar hijo\n";

    // Testing parent/child position
    std::cout << "\n--- TESTING PARENT/CHILD TRANSFORM ---\n";

	sf::Vector2f parentPos(50.f, 50.f);

	transform->set_parent(parentEntity);
	parentTransform->set_position(parentPos);
	transform->set_local_position(sf::Vector2f(5.f, 5.f));

	std::cout << "Posicion del padre: (" << parentTransform->get_position().x << ", " << parentTransform->get_position().y << ")\n";
	std::cout << "Posicion local del hijo: (" << transform->get_local_position().x << ", " << transform->get_local_position().y << ")\n";
	std::cout << "Posicion global del hijo: (" << transform->get_position().x << ", " << transform->get_position().y << ")\n";


    // Set parent and initial values
    transform->set_parent(parentEntity);
    parentTransform->set_scale(sf::Vector2f(2.f, 2.f));
    parentTransform->set_rotation(30.f);

    // Set local scale and rotation for child
    transform->set_local_scale(sf::Vector2f(1.5f, 1.5f));
    transform->set_local_rotation(45.f);

    std::cout << "Escala del padre: (" << parentTransform->get_scale().x << ", " << parentTransform->get_scale().y << ")\n";
    std::cout << "Escala local del hijo: (" << transform->get_local_scale().x << ", " << transform->get_local_scale().y << ")\n";
    std::cout << "Escala global del hijo: (" << transform->get_scale().x << ", " << transform->get_scale().y << ")\n";

    std::cout << "Rotacion del padre: " << parentTransform->get_rotation() << "\n";
    std::cout << "Rotacion local del hijo: " << transform->get_local_rotation() << "\n";
    std::cout << "Rotacion global del hijo: " << transform->get_rotation() << "\n";

    std::cout << "--- FIN TESTS DE TRANSFORM ---\n\n";


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
