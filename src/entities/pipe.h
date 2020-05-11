#pragma once

#include <SFML/Graphics.hpp>

#include "entities/wallobject.h"
#include "map/map.h"

class Pipe : public WallObject {
   private:
    static sf::Texture assetGreenTexture, assetOrangeTexture;
    sf::Sprite sprite;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roiGreen,
                           const sf::IntRect &roiOrange);

    Pipe(const sf::Vector2f &position, bool greenNotOrange);

    sf::Sprite &getSprite() { return sprite; }
};