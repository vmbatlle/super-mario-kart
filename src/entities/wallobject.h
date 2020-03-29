#pragma once

class WallObject;
#include <memory>
typedef std::shared_ptr<WallObject> WallObjectPtr;

#include <SFML/Graphics.hpp>

class WallObject {
   public:
    sf::Vector2f position;  // x, y
    float radius;
    float height;

    WallObject(const sf::Vector2f _position, const float _radius,
               const float _height, const int mapWidth, const int mapHeight);

    virtual void update(const sf::Time &) {}
    virtual sf::Sprite &getSprite() = 0;
};