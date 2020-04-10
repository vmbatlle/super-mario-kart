#pragma once

class WallObject;
#include <memory>
typedef std::shared_ptr<WallObject> WallObjectPtr;

#include <SFML/Graphics.hpp>
#include <cmath>
#include "entities/collisiondata.h"

class WallObject {
   public:
    sf::Vector2f position;  // x, y
    float visualRadius, hitboxRadius;
    float height;

    WallObject(const sf::Vector2f &_position, const float _visualRadius,
               const float _hitboxRadius, const float _height,
               const int mapWidth, const int mapHeight);

    virtual void update(const sf::Time &) {}
    virtual sf::Sprite &getSprite() = 0;

    // default collision handling assumes i'm fixed and just moves other object
    // this only works with static objects e.g. pipes
    virtual bool solveCollision(CollisionData &data,
                                const sf::Vector2f &otherSpeed,
                                const sf::Vector2f &otherPos,
                                const float otherWeight, const float distance2);
};