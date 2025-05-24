#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    // Create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test Window");
    window.setFramerateLimit(60);
    
    // Create a circle shape
    sf::CircleShape circle(50.f);
    circle.setFillColor(sf::Color(100, 250, 50));
    circle.setPosition(100.f, 100.f);
    
    // Create a text object
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        // Continue anyway, we'll just not see the text
    }
    
    sf::Text text("SFML is working!", font, 24);
    text.setFillColor(sf::Color::White);
    text.setPosition(250.f, 300.f);
    
    // Movement speed
    float speed = 5.0f;
    sf::Vector2f movement(speed, speed);
    
    // Main loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
                
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
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
