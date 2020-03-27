#pragma once

class Driver;
#include <memory>
typedef std::shared_ptr<Driver> DriverPtr;

#include <SFML/Graphics.hpp>
#include <cmath>
#include "entities/driveranimator.h"
#include "input/input.h"

class Driver {
   public:
    DriverAnimator animator;
    sf::Vector2f position;
    float posAngle;
    float height;
    float speedForward, speedTurn;
    int rounds;

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle)
        : animator(spriteFile),
          position(initialPosition),
          posAngle(initialAngle),
          height(0.0f),
          speedForward(0.0f),
          speedTurn(0.0f),
          rounds(0) {}

    void update(const sf::Time &deltaTime);
    std::pair<float, sf::Sprite *> getDrawable(const sf::RenderTarget &window);
};