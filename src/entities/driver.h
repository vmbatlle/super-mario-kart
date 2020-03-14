#pragma once

class Driver;

#include <SFML/Graphics.hpp>
#include <cmath>
#include "entities/driveranimator.h"
#include "input/input.h"
#include "map/map.h"

class Driver {
   public:
    DriverAnimator animator;
    sf::Vector2f position;
    int rounds;
    float posAngle;
    float speedForward, speedTurn;

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle)
        : animator(spriteFile),
          position(initialPosition),
          rounds(0),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f) {}

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};