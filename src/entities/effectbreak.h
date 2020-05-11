#pragma once

#include <SFML/Graphics.hpp>

#include "entities/item.h"

class EffectBreak : public Item {
   private:
    static constexpr const float JUMP_SPEED = 75.0f;
    static constexpr const float GRAVITY = -9.8f * 35.0f;
    float verticalSpeed;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi0, const sf::IntRect &roi1,
                           const sf::IntRect &roi2, const sf::IntRect &roi3,
                           const sf::IntRect &roi4);

    EffectBreak(Item *item);

    // moves item (doesn't do collision)
    void update(const sf::Time &deltaTime) override;

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;

    inline std::string name() const override { return "EffectBreak"; }
};