//
//  App.hpp
//  SFMLClient
//
//  Created by Dev on 12/01/2023.
//  Copyright © 2023 Dev. All rights reserved.
//

#ifndef App_hpp
#define App_hpp

#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class App
{
private:
    sf::RenderWindow* window;
    sf::Font* font;
    
public:
    App();
    ~App();
    
    void Run(void);
};

#endif /* App_hpp */
