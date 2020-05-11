#pragma once

#include <SFML/Graphics.hpp>

#include "entities/item.h"

class FloatingFish : public Item {
   private:
    static sf::Image assetFishImage;
    static sf::IntRect roi1L, roi2L, roi1R, roi2R;
    std::array<sf::Texture, 2> assetFishTextures;
    sf::Time totalTime;
    sf::Vector2f speed;
    bool usingFirstTexture;
    bool flipped;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &_roi1L, const sf::IntRect &_roi2L,
                           const sf::IntRect &_roi1R,
                           const sf::IntRect &_roi2R);

    FloatingFish(const sf::Vector2f &position, const sf::Vector2f &_speed,
                 bool _flipped);

    void update(const sf::Time &deltaTime) override;

    inline std::string name() const override { return "FloatingFish"; }
};