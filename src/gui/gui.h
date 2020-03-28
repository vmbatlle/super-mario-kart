#pragma once

#include <SFML/Graphics.hpp>
#include "timer.h"

class Gui {
    public:

    Timer timer;

    sf::Vector2u winSize;

    Gui();

    void setWindowSize(sf::Vector2u s);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};