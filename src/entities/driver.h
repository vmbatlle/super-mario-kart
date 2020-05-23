#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

class Driver;
#include <memory>
typedef std::shared_ptr<Driver> DriverPtr;

#include <stdio.h>

#include <SFML/Graphics.hpp>
#include <list>

typedef std::vector<sf::Vector2f>::const_iterator PathIterator;

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
    MORE_SPEED_UP = 2,
    SPEED_DOWN = 4,
    STAR = 8,
    UNCONTROLLED = 16,
    STOPPED = 32,
    INVISIBLE = 64,
    _COUNT = 8,
};

class Driver : public WallObject {
   public:
    static constexpr const float HITBOX_RADIUS = 1.5f;

   private:
    static const sf::Time SPEED_UP_DURATION;
    static const sf::Time MORE_SPEED_UP_DURATION;
    static const sf::Time SPEED_DOWN_DURATION;
    static const sf::Time STAR_DURATION;
    static const sf::Time UNCONTROLLED_DURATION;

    static const float COIN_SPEED;

    MenuPlayer pj;  // controlled character e.g. mario, etc.
    int laps = 0;   // lap number (0: race start, 1: first lap, 5: last lap)
    int maxLapSoFar = 0;  // max lap on this race (for backwards)

    int lastGradient;                   // gradient value in last update cycle
    int consecutiveGradientIncrements;  // checks if player is going backwards
                                        // if its gradient keeps going up

    int coins = 0;
    static const sf::Time ITEM_USE_WAIT;
    PowerUps powerUp = PowerUps::NONE;
    sf::Time canUseItemAt = sf::Time::Zero;

    static const sf::Time FOLLOWED_PATH_UPDATE_INTERVAL;
    static const int STEPS_BACK_FOR_RELOCATION;
    static const int STEPS_STILL_FOR_RELOCATION;
    std::vector<sf::Vector2f> followedPath;  // Prev. positions of the driver
    std::vector<int> prevLap;                // Lap at that position
    std::vector<int> indexOfLap;  // First `followedPath` index at lap i + 1.
    sf::Time pathLastUpdatedAt;   // Time of last stored position

    int state = (int)DriverState::NORMAL;
    sf::Time stateEnd[(int)DriverState::_COUNT] = {sf::seconds(0)};

   public:
    // push a `time` to delete `state`
    void pushStateEnd(DriverState state, const sf::Time &endTime);

    // pop state from stack
    void popStateEnd(DriverState state);

   private:
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
    bool falling = false;
    bool pressedToDrift = false;
    bool heightByRamp = false;
    bool goingForwards = true;  // true if driver has been following gradient
    sf::Vector2f vectorialSpeed;
    sf::Vector2f collisionMomentum;
    DriverControlType controlType;
    const VehicleProperties *vehicle;
    const RaceRankingArray &positions;
    bool isRealPlayer;
    static DriverPtr realPlayer;
    int rank;  // this is here for question panels,
               // RaceRankArray should be used instead
    int farVisionModifier = 0;
    float itemProbModifier = 1;
    unsigned int impedimentModifier = 0;

    bool deletePositions;

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle, const int mapWidth, const int mapHeight,
           const DriverControlType _controlType,
           const VehicleProperties &_vehicle, const MenuPlayer _pj,
           const RaceRankingArray &_positions, bool _isRealPlayer = false,
           int farVisionMod = 0, float itemProbMod = 1,
           unsigned int impedimentMod = 8)
        : WallObject(initialPosition, 1.0f, HITBOX_RADIUS, 0.0f, mapWidth,
                     mapHeight),
          pj(_pj),
          animator(spriteFile, controlType),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f),
          speedUpwards(0.0f),
          collisionMomentum(0.0f, 0.0f),
          controlType(_controlType),
          vehicle(&_vehicle),
          positions(_positions),
          isRealPlayer(_isRealPlayer),
          farVisionModifier(farVisionMod),
          itemProbModifier(itemProbMod),
          impedimentModifier(impedimentMod),
          deletePositions(false) {}

    Driver(const char *spriteFile, const sf::Vector2f &initialPosition,
           const float initialAngle, const int mapWidth, const int mapHeight,
           const DriverControlType _controlType,
           const VehicleProperties &_vehicle, const MenuPlayer _pj)
        : WallObject(initialPosition, 1.0f, HITBOX_RADIUS, 0.0f, mapWidth,
                     mapHeight),
          pj(_pj),
          animator(spriteFile, controlType),
          posAngle(initialAngle),
          speedForward(0.0f),
          speedTurn(0.0f),
          speedUpwards(0.0f),
          collisionMomentum(0.0f, 0.0f),
          controlType(_controlType),
          vehicle(&_vehicle),
          positions(*(new RaceRankingArray())),
          isRealPlayer(false),
          deletePositions(true) {}

    ~Driver() {
        if (deletePositions) {
            delete &positions;
        }
    }

    // item-related methods
    void applyMushroom();
    void applyStar();
    void applyThunder(sf::Time duration);

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

    bool canDrive() const;
    bool isImmune() const;
    bool isVisible() const;
    bool canUsePowerUp() const;
    void pickUpPowerUp(PowerUps power);
    inline PowerUps getPowerUp() const { return powerUp; }
    inline int getRank() const { return rank; }

    void reset();
    void endRaceAndReset();
    void setPositionAndReset(const sf::Vector2f &newPosition,
                             const float newAngle = M_PI_2 * -1.0f);

    void updateSpeed(const sf::Time &deltaTime);
    void update(const sf::Time &deltaTime) override;

    sf::Sprite &getSprite() override;
    void getDrawables(const sf::RenderTarget &window, const float scale,
                      std::vector<std::pair<float, sf::Sprite *>> &drawables,
                      const bool withShadow = true);

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;

    // iterator to positions of player in lap-th lap (1-index).
    void getLapTrajectory(unsigned int lap, PathIterator &begin,
                          PathIterator &end);

    // get a position to teleport player if outlimits or blocked
    void relocateToNearestGoodPosition();
};