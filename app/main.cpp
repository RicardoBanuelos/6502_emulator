#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    // Create a window
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 800)), "SFML Test Window");
    window.setFramerateLimit(60);
    window.setPosition(sf::Vector2i(100, 100));

    // Create a circle shape
    sf::CircleShape circle(50.f);
    circle.setFillColor(sf::Color(100, 250, 50));
    circle.setPosition(sf::Vector2f(150.f, 100.f));

    // Create a text object
    sf::Font font;

    if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
    {
        std::cerr << "Error loading font" << std::endl;
        // Continue anyway, we'll just not see the text
    }

    sf::Text text(font, "SFML is working!", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(250.f, 300.f));

    // Movement speed
    float speed = 5.0f;
    sf::Vector2f movement(speed, speed);

    // Main loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }

        // Move the circle
        sf::Vector2f position = circle.getPosition();
        if (position.x < 0 || position.x > window.getSize().x - 100)
            movement.x = -movement.x;
        if (position.y < 0 || position.y > window.getSize().y - 100)
            movement.y = -movement.y;

        circle.move(movement);

        // Clear, draw, display
        window.clear(sf::Color(50, 50, 50));
        window.draw(circle);
        window.draw(text);
        window.display();
    }

    return 0;
}
