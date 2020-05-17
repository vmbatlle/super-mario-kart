#pragma once

#include <SFML/Graphics.hpp>

#include "entities/driver.h"
#include "entities/item.h"
#include "map/map.h"

class RedShell : public Item {
   private:
    static constexpr const float SPEED = 75.0f;
    static constexpr const float HITBOX_RADIUS = 4.0f;
    static constexpr const float MAX_HEIGHT = 8.0f;
    static const sf::Time TIME_OF_FLIGHT;
    static sf::Texture assetShell;
    static unsigned int numRedShellsFollowingPlayer;
    bool followingPlayer;
    const Driver *target;
    int inactiveFrames;  // if >0, collisions don't count (dont hit own thrower)
    sf::Vector2f speed;
    sf::Time flightRemainingTime = sf::seconds(0.0f);
    sf::Vector2f lastDirection = sf::Vector2f(0.0f, 0.0f);
    int gradientWhenRamp = -1;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi);

    RedShell(const sf::Vector2f &_position, const Driver *_target,
             const float forwardAngle, const bool forwardThrow);
    ~RedShell();

    void update(const sf::Time &deltaTime) override;

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;

    bool registersCollisions() const override { return true; }

    inline std::string name() const override { return "RedShell"; }
};