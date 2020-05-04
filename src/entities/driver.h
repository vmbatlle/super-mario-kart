#pragma once

class Driver;
#include <memory>
typedef std::shared_ptr<Driver> DriverPtr;

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <list>

#include "audio/audio.h"
#include "entities/driveranimator.h"
#include "entities/enums.h"
#include "entities/vehicleproperties.h"
#include "entities/wallobject.h"
#include "input/input.h"
#include "map/enums.h"

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

    static const float COIN_SPEED;

    MenuPlayer pj;  // controlled character e.g. mario, etc.
    int laps = 0;   // lap number (0: race start, 1: first lap, 5: last lap)

    int lastGradient;                   // gradient value in last update cycle
    int consecutiveGradientIncrements;  // checks if player is going backwards
                                        // if its gradient keeps going up

    int coins = 0;
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

    // update gradient and lap related stuff
    void updateGradientPosition();

   public:
    // position, height & visual/hitbox radius are inherited
    DriverAnimator animator;
    float posAngle, flightAngle;
    float speedForward, speedTurn;
    float speedUpwards;
    bool onLakitu = false;
    bool pressedToDrift = false;
    bool goingForwards = true;  // true if driver has been following gradient
    sf::Vector2f vectorialSpeed;
    sf::Vector2f collisionMomentum;
    DriverControlType controlType;
    const VehicleProperties *vehicle;
    int rank;  // this is here for question panels,
               // RaceRankArray should be used instead

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle, const int mapWidth, const int mapHeight,
           const DriverControlType _controlType,
           const VehicleProperties &_vehicle, const MenuPlayer _pj)
        : WallObject(initialPosition, 1.0f, HITBOX_RADIUS, 0.0f, mapWidth,
                     mapHeight),
          pj(_pj),
          animator(spriteFile, _controlType),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f),
          speedUpwards(0.0f),
          collisionMomentum(0.0f, 0.0f),
          controlType(_controlType),
          vehicle(&_vehicle) {}

    // item-related methods
    void applyMushroom();
    void applyStar();
    void applyThunder();

    // animator-related methods
    void shortJump();
    void jumpRamp(const MapLand &land);
    void applyHit();
    void applySmash();

    void addCoin(int amount = 1);
    inline int getCoins() const { return coins; }
    inline int getLaps() const { return laps; }
    inline int getLastGradient() const { return lastGradient; }
    inline MenuPlayer getPj() const { return pj; }

    inline bool isGoingForward() const { return goingForwards; }
    inline bool isGoingBackwards() const { return !isGoingForward(); }

    bool canDrive();
    void pickUpPowerUp(PowerUps power);
    inline PowerUps getPowerUp() const { return powerUp; }

    void endRaceAndReset();
    void setPositionAndReset(const sf::Vector2f &newPosition);

    void update(const sf::Time &deltaTime) override;

    sf::Sprite &getSprite() override;
    std::pair<float, sf::Sprite *> getDrawable(const sf::RenderTarget &window,
                                               const float scale);

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float distance2) override;
};