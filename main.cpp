#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>

int main() {
	std::cout << "Hello, World!" << std::endl;
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
		// Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();

        // Update

		// Render
    }
	return 0;
}