#pragma once

#include <SFML/Graphics.hpp>

#include "entities/item.h"

class EffectCoin : public Item {
   private:
    static constexpr const float JUMP_SPEED = 110.0f;
    static constexpr const float GRAVITY = -9.8f * 35.0f;
    static constexpr const unsigned int NUM_TEXTURES = 4;
    static std::array<sf::Texture, NUM_TEXTURES> assetTextures;

    // position
    const Driver *driver;
    float relativeHeight;  // relative to driver
    sf::Time delay;
    const bool positive;
    float verticalSpeed;
    float speedForward, posAngle;

    // textures
    static const sf::Time TIME_BETWEEN_FRAMES;
    sf::Time currentTime;
    unsigned int currentFrame;
    bool once;

    void setTexture(const int frame);

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi0, const sf::IntRect &roi1,
                           const sf::IntRect &roi2);

    EffectCoin(const Driver *_driver, const sf::Time &_delay,
               const bool _positive);

    // moves item (doesn't do collision)
    void update(const sf::Time &deltaTime) override;

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;

    inline std::string name() const override { return "EffectCoin"; }
};