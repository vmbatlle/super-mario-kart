#pragma once

#include <SFML/Graphics.hpp>

#include "entities/driver.h"
#include "entities/item.h"
#include "map/map.h"

class Banana : public Item {
   private:
    static constexpr const float SPEED = 60.0f;
    static constexpr const float HITBOX_RADIUS = 0.8f;
    static constexpr const float GRAVITY = -9.8f * SPEED * 1.5f;  // lol
    static sf::Texture assetBanana;
    sf::Vector3f speed;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi);

    Banana(const sf::Vector2f &_position, const float forwardAngle,
           const bool forwardThrow, const float playerHeight);

    void update(const sf::Time &deltaTime) override;

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;

    bool registersCollisions() const override { return true; }

    inline std::string name() const override { return "Banana"; }
};