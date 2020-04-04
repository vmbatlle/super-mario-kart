#pragma once

class Driver;
#include <memory>
typedef std::shared_ptr<Driver> DriverPtr;

#include <SFML/Graphics.hpp>
#include <cmath>
#include <list>
#include <stdio.h>

#include "entities/driveranimator.h"
#include "entities/enums.h"
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
    // TODO: make all of this vehicle-specific
    static constexpr const float MAX_NORMAL_LINEAR_SPEED = 0.1f;
    static constexpr const float MAX_SPEED_UP_LINEAR_SPEED = 0.2f;
    static constexpr const float MAX_SPEED_DOWN_LINEAR_SPEED = 0.05f;

    static constexpr const float FRICTION_LINEAR_ACELERATION = -0.03f;
    static constexpr const float MOTOR_ACELERATION = 0.1f;
    static constexpr const float BREAK_ACELERATION = -0.1f;

    static constexpr const float SLOW_LAND_MAX_LINEAR_SPEED =
        MAX_NORMAL_LINEAR_SPEED / 2.0f;
    static constexpr const float SLOW_LAND_LINEAR_ACELERATION = -0.15f;

    // TODO: make it depend on the object
    static const sf::Time SPEED_UP_DURATION;
    static const sf::Time SPEED_DOWN_DURATION;
    static const sf::Time STAR_DURATION;
    static const sf::Time UNCONTROLLED_DURATION;

    int coints = 0;
    PowerUps powerUp = PowerUps::NONE;

    int state = (int)DriverState::NORMAL;
    sf::Time stateEnd[(int)DriverState::_COUNT] = {sf::seconds(0)};


    // push a `time` to delete `state`
    void pushStateEnd(DriverState state, const sf::Time &endTime);

    // pop finished states if any
    int popStateEnd(const sf::Time &currentTime);

    // update using input service
    void usePlayerControls(float &acceleration);

    // update based on gradient AI
    void useGradientControls(float &acceleration);

   public:
    // position, height & radius are inherited
    DriverAnimator animator;
    float posAngle;
    float speedForward, speedTurn;
    int rounds;
    DriverControlType controlType;

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle, const int mapWidth, const int mapHeight,
           const DriverControlType _controlType)
        : WallObject(initialPosition, 1.0f, 0.0f, mapWidth, mapHeight),
          animator(spriteFile),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f),
          rounds(0),
          controlType(_controlType) {}

    void addCoin();
    int getCoins();

    void pickUpPowerUp(PowerUps power);
    PowerUps getPowerUp();

    void update(const sf::Time &deltaTime) override;
    sf::Sprite &getSprite() override;
    std::pair<float, sf::Sprite *> getDrawable(const sf::RenderTarget &window);
};