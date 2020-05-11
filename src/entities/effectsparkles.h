#pragma once

#include <SFML/Graphics.hpp>

#include "entities/item.h"

class EffectSparkles : public Item {
   private:
    // all 4 sparkle textures
    // https://www.spriters-resource.com/snes/smariokart/sheet/107671/
    static constexpr const int NUM_TEXTURES = 4;
    static std::array<sf::Texture, NUM_TEXTURES> assetTextures;

    static const sf::Time TIME_BETWEEN_FRAMES;

    sf::Time currentTime;
    int currentFrame;

    void setTexture(const int frame);

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi0, const sf::IntRect &roi1,
                           const sf::IntRect &roi2, const sf::IntRect &roi3);

    EffectSparkles(const sf::Vector2f &_position)
        : Item(sf::Vector2f(_position.x * MAP_ASSETS_WIDTH,
                            _position.y * MAP_ASSETS_HEIGHT),
               1.05f,  // a bit bigger than driver's visual radius
                       // so it goes in front of him
               0.0f, 0.0f),
          currentTime(sf::Time::Zero),
          currentFrame(0) {
        setTexture(0);
    }

    // moves item (doesn't do collision)
    void update(const sf::Time &deltaTime) override;

    // return CollsionData if this object collides WITH A DRIVER
    bool solveCollision(CollisionData &data, const sf::Vector2f &otherSpeed,
                        const sf::Vector2f &otherPos, const float otherWeight,
                        const float otherHeight, const bool otherIsImmune,
                        const float distance2) override;

    inline std::string name() const override { return "EffectSparkles"; }
};