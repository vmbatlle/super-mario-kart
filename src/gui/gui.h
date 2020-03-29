#pragma once

#include <SFML/Graphics.hpp>
#include "timer.h"
#include "itemIndicator.h"

class Gui {
    public:

    Timer timer;
    ItemIndicator itemInd;

    sf::Vector2u winSize;

    Gui();

    void setWindowSize(sf::Vector2u s);

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};