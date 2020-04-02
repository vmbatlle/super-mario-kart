#pragma once

class Driver;
#include <memory>
typedef std::shared_ptr<Driver> DriverPtr;

#include <SFML/Graphics.hpp>
#include <cmath>
#include <list>

#include "entities/driveranimator.h"
#include "entities/wallobject.h"
#include "input/input.h"

enum class DriverState : int {
    NORMAL = 0,
    SPEED_UP = 1,
    SPEED_DOWN = 2,
    STAR = 4,
    UNCONTROLLED = 8,
    _COUNT = 4,
};

class Driver : public WallObject {
   private:
    const float MAX_NORMAL_LINEAR_SPEED = 0.1f;
    const float MAX_SPEED_UP_LINEAR_SPEED = 0.2f;
    const float MAX_SPEED_DOWN_LINEAR_SPEED = 0.05f;

    const float SLOW_LAND_MAX_LINEAR_SPEED = MAX_NORMAL_LINEAR_SPEED / 2.0f;
    const float SLOW_LAND_LINEAR_ACELERATION = -0.15f;

    // TODO: make it depende on the object
    const sf::Time SPEED_UP_DURATION = sf::seconds(1.5f);
    const sf::Time SPEED_DOWN_DURATION = sf::seconds(20.0f);
    const sf::Time STAR_DURATION = sf::seconds(30.0f);
    const sf::Time UNCONTROLLED_DURATION = sf::seconds(1.0f);

    int coints = 0;
    int state = (int)DriverState::NORMAL;
    sf::Time stateEnd[(int)DriverState::_COUNT] = {sf::seconds(0)};

    // push a `time` to delete `state`
    void pushStateEnd(DriverState state, const sf::Time &endTime);

    // pop finished states if any
    int popStateEnd(const sf::Time &currentTime);

   public:
    // position, height & radius are inherited
    DriverAnimator animator;
    float posAngle;
    float speedForward, speedTurn;
    int rounds;

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle, const int mapWidth, const int mapHeight)
        : WallObject(initialPosition, 4.0f, 0.0f, mapWidth, mapHeight),
          animator(spriteFile),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f),
          rounds(0) {}

    void update(const sf::Time &deltaTime) override;
    sf::Sprite &getSprite() override;
    std::pair<float, sf::Sprite *> getDrawable(const sf::RenderTarget &window);
};