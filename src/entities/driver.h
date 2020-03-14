#pragma once

class Driver;
#include <memory>
typedef std::shared_ptr<Driver> DriverPtr;

#include <SFML/Graphics.hpp>
#include <cmath>
#include "entities/driveranimator.h"
#include "input/input.h"
#include "map/map.h"

class Driver {
   public:
    DriverAnimator animator;
    sf::Vector2f position;
    float posAngle;
    float speedForward, speedTurn;

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle)
        : animator(spriteFile),
          position(initialPosition),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f) {}

    void update(const sf::Time &deltaTime);
    void draw(sf::RenderTarget &window);
};