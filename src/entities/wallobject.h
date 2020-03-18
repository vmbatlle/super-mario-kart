#pragma once

class WallObject;
#include <memory>
typedef std::shared_ptr<WallObject> WallObjectPtr;

#include <SFML/Graphics.hpp>

class WallObject {
   public:
    sf::Vector2f position;  // x, y
    float height;

    WallObject(const sf::Vector2f _position, const float _height)
        : position(_position), height(_height) {}

    virtual sf::Sprite &getSprite() = 0;
};