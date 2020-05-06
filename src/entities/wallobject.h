#pragma once

class WallObject;
#include <memory>
typedef std::shared_ptr<WallObject> WallObjectPtr;

#include <SFML/Graphics.hpp>
#include <cmath>
#include "entities/collisiondata.h"

class WallObject {
   public:
    static sf::Texture assetShadow;
    sf::Sprite spriteShadow;

    sf::Vector2f position;  // x, y
    float visualRadius, hitboxRadius;
    float height;

    static void loadAssets(const std::string& assetName);

    WallObject(const sf::Vector2f &_position, const float _visualRadius,
               const float _hitboxRadius, const float _height,
               const int mapWidth, const int mapHeight);

    virtual void update(const sf::Time &) {}
    virtual sf::Sprite &getSprite() = 0;

    // default collision handling assumes i'm fixed and just moves other object
    // this only works with static objects e.g. pipes
    static bool defaultSolveCollision(CollisionData &data,
                                      const sf::Vector2f &otherSpeed,
                                      const sf::Vector2f &otherPos,
                                      const sf::Vector2f &myPos,
                                      const float distance2);

    // return CollsionData if this object collides WITH A DRIVER
    virtual bool solveCollision(CollisionData &data,
                                const sf::Vector2f &otherSpeed,
                                const sf::Vector2f &otherPos,
                                const float otherWeight,
                                const bool otherIsImmune,
                                const float distance2);
};