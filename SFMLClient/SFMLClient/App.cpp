//
//  App.cpp
//  SFMLClient
//
//  Created by Dev on 12/01/2023.
//  Copyright © 2023 Dev. All rights reserved.
//

#include "App.hpp"
#include "ResourcePath.hpp"

App::App()
{
    sf::RenderWindow* window = new sf::RenderWindow;
    window->create(sf::VideoMode(1200, 800), "SFML window");
    
    sf::Font* font = new sf::Font;
    if (!font->loadFromFile(resourcePath() + "dos_vga_font.ttf"))
    {
        return EXIT_FAILURE;
    }
    
    this->window = window;
    this->font = font;
    
    // setup a client
    // setup UI
}

App::~App()
{
    
}

void App::Run(void)
{
    
    sf::Text sampleText;
    
    sampleText.setFont(*this->font);
    sampleText.setString("CLIENT");
    sampleText.setCharacterSize(16);
    sampleText.setFillColor(sf::Color::Red);
    sampleText.setPosition(600, 400);
    
    while (window->isOpen())
    {
        // Process events
        sf::Event event;
        while (window->pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window->close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window->close();
            }
        }

        // Clear screen
        window->clear();
        
        // Draw sample text
        window->draw(sampleText);
        
        // Update the window
        window->display();
    }
}
