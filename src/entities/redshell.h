#pragma once

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "entities/item.h"
#include "map/map.h"

class RedShell : public Item {
   private:
    static constexpr const float SPEED = 75.0f;
    static constexpr const float HITBOX_RADIUS = 4.0f;
    static sf::Texture assetShell;
    const Driver *target;
    int inactiveFrames;  // if >0, collisions don't count (dont hit own thrower)
    sf::Vector2f speed;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi);

    RedShell(const sf::Vector2f &_position, const Driver *_target,
             const float forwardAngle, const bool forwardThrow);

    void update(const sf::Time &deltaTime) override;

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const bool otherIsImmune,
                        const float distance2) override;
};