#pragma once

#include <memory>
class EndRanks;
typedef std::shared_ptr<EndRanks> EndRanksPtr;

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "entities/driver.h"
#include "entities/enums.h"

class EndRanks {
   private:
    static constexpr const unsigned int NUM_TEXTURES = 8;
    // driver faces
    static std::array<sf::Texture, NUM_TEXTURES> assetsHappy, assetsSad,
        assetsNormal;
    // ranks 1-8
    static std::array<sf::Texture, NUM_TEXTURES> numbersBlack, numbersYellow,
        numbersRed;
    static float FACE_DY, NUMBER_DX;

    static constexpr const float NORMAL_WIDTH = 256.0f, NORMAL_HEIGHT = 224.0f;
    static const sf::Vector2f ABS_POSITION;

    static const RaceRankingArray *ranks;
    static unsigned int framesSinceOrigin;

   public:
    static void loadAssets(
        const std::string &assetName, const sf::IntRect &roiHappy0,
        const sf::IntRect &roiSad0, const sf::IntRect &roiNormal0,
        const unsigned int roiFaceDY, const sf::IntRect &roiBlack0,
        const sf::IntRect &roiYellow0, const sf::IntRect &roiRed0,
        const unsigned int roiNumberDX);

    static void update(const sf::Time &deltaTime);
    static void draw(sf::RenderTarget &window);

    static void reset(const RaceRankingArray *_ranks);
};