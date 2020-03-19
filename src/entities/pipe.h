#pragma once

#include <SFML/Graphics.hpp>
#include "entities/wallobject.h"

class Pipe : public WallObject {
   private:
    static sf::Texture assetTexture;
    sf::Sprite sprite;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roi) {
        assetTexture.loadFromFile(assetName, roi);
    }

    Pipe(const sf::Vector2f &position)
        : WallObject(position, 0.0f), sprite(assetTexture) {
        sf::Vector2u size = assetTexture.getSize();
        sprite.setOrigin(size.x / 2.0f, size.y);
    }
    sf::Sprite &getSprite() { return sprite; }
};