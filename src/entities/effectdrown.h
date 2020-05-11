#pragma once

#include <SFML/Graphics.hpp>

#include "entities/item.h"

class EffectDrown : public Item {
   private:
    // all 5 water sprites
    // https://www.spriters-resource.com/snes/smariokart/sheet/107671/
    static constexpr const int NUM_TEXTURES = 5;
    static std::array<sf::Texture, NUM_TEXTURES> assetTextures;

    static const sf::Time TIME_BETWEEN_FRAMES;

    const sf::Color tint;
    sf::Time currentTime;
    int currentFrame;

    void setTexture(const int frame);

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi0, const sf::IntRect &roi1,
                           const sf::IntRect &roi2, const sf::IntRect &roi3,
                           const sf::IntRect &roi4);

    EffectDrown(const sf::Vector2f &_position, const sf::Color &_tint)
        : Item(sf::Vector2f(_position.x * MAP_ASSETS_WIDTH,
                            _position.y * MAP_ASSETS_HEIGHT),
               1.05f,  // a bit bigger than driver's visual radius
                       // so it goes in front of him
               0.0f, 0.0f),
          tint(_tint),
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

    inline std::string name() const override { return "EffectBreak"; }
};