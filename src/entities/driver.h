#pragma once

class Driver;
#include <memory>
typedef std::shared_ptr<Driver> DriverPtr;

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <list>

#include "entities/driveranimator.h"
#include "entities/enums.h"
#include "entities/vehicleproperties.h"
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
   public:
    static constexpr const float HITBOX_RADIUS = 1.5f;

   private:
    // TODO: make it depend on the object
    static const sf::Time SPEED_UP_DURATION;
    static const sf::Time SPEED_DOWN_DURATION;
    static const sf::Time STAR_DURATION;
    static const sf::Time UNCONTROLLED_DURATION;

    MenuPlayer pj;

    int rank = 1;
    int laps = 1;

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
    // position, height & visual/hitbox radius are inherited
    DriverAnimator animator;
    float posAngle;
    float speedForward, speedTurn;
    sf::Vector2f collisionMomentum;
    DriverControlType controlType;
    const VehicleProperties &vehicle;

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle, const int mapWidth, const int mapHeight,
           const DriverControlType _controlType,
           const VehicleProperties &_vehicle, const MenuPlayer _pj)
        : WallObject(initialPosition, 1.0f, HITBOX_RADIUS, 0.0f, mapWidth,
                     mapHeight),
          pj(_pj),
          animator(spriteFile),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f),
          collisionMomentum(0.0f, 0.0f),
          controlType(_controlType),
          vehicle(_vehicle) {}

    void addCoin(int ammount = 1);
    int getCoins();

    void setBonnusSpeed(float factor);

    void addLap();
    int getLaps();

    void setRank(int r);
    int getRank();

    MenuPlayer getPj();

    void pickUpPowerUp(PowerUps power);
    PowerUps getPowerUp();

    void update(const sf::Time &deltaTime) override;
    sf::Sprite &getSprite() override;
    std::pair<float, sf::Sprite *> getDrawable(const sf::RenderTarget &window);

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float distance2) override;
};