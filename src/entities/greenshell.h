#pragma once

#include <SFML/Graphics.hpp>

#include "entities/driver.h"
#include "entities/item.h"
#include "map/map.h"

class GreenShell : public Item {
   private:
    static constexpr const int NUM_LIVES = 6;
    static constexpr const int NUM_MARCHES_UPDATE = 3;
    static constexpr const int NUM_MARCHES_HIT = 20;
    static constexpr const float SPEED = 80.0f;
    static constexpr const float HITBOX_RADIUS = 5.0f;
    static constexpr const float JUMP_SPEED = 150.0f;
    static constexpr const float GRAVITY = -9.8f * 100.0f;  // lol
    static const sf::Time TIME_OF_FLIGHT;
    static sf::Texture assetShell;
    sf::Vector2f speed;
    float verticalSpeed;
    int lives;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi);

    GreenShell(const sf::Vector2f &_position, const float forwardAngle,
               const bool forwardThrow, const float playerHeight);

    void update(const sf::Time &deltaTime) override;
    void marchingUpdate(const sf::Time &deltaTime);

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;

    bool registersCollisions() const override { return true; }

    inline std::string name() const override { return "GreenShell"; }
};