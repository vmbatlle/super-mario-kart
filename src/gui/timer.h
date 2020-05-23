#pragma once

#include <SFML/Graphics.hpp>

#include "entities/enums.h"

class Timer {
   public:
    sf::Texture digits[10];  // 0 1 2 3 4 5 6 7 8 9
    sf::Texture comas[2];    // ' ''
    sf::Texture custom[3];   // : - .

    sf::Sprite timerDigits[6];
    sf::Sprite timerCommas[2];

    sf::Vector2f leftUpCorner;
    sf::Vector2u winSize;

    sf::Time time;

    float factor;
    sf::Vector2f scaleFactor;

    Timer();

    void setWindowSize(sf::Vector2u s);
    sf::Vector2f getItemPos();

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window,
              const sf::Color &color);

    void reset();
};