#pragma once

class Gui;

#include <SFML/Graphics.hpp>
#include "timer.h"
#include "itemIndicator.h"
#include "others.h"
#include "../entities/enums.h"

class Gui {
    private:

    Gui();
    static Gui instance;

    public:

    Timer timer;
    ItemIndicator itemInd;
    Others others;

    sf::Vector2u winSize;

    static void setPowerUp(PowerUps power);
    static void addCoin();

    static void setWindowSize(sf::Vector2u s);

    static void update(const sf::Time &deltaTime);
    static void draw(sf::RenderTarget &window);
};