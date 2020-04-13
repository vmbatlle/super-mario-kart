#pragma once

#include <SFML/Graphics.hpp>
#include "entities/driver.h"
#include "entities/item.h"

class Banana : public Item {
   private:
    static constexpr const float SPEED = 100.0f;
    static constexpr const float HITBOX_RADIUS = 0.3f;
    static constexpr const float GRAVITY = -9.8f * SPEED * 1.5f;
    static sf::Texture assetBanana;
    sf::Vector3f speed;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi);

    Banana(const sf::Vector2f &_position, const float forwardAngle,
           const bool forwardThrow);

    void update(const sf::Time &deltaTime) override;
};