#include "wallobject.h"

sf::Texture WallObject::assetShadow;

void WallObject::loadAssets(const std::string &assetName) {
    assetShadow.loadFromFile(assetName);
}

WallObject::WallObject(const sf::Vector2f &_position, const float _visualRadius,
                       const float _hitboxRadius, const float _height,
                       const int mapWidth, const int mapHeight)
    : visualRadius(_visualRadius / mapWidth),
      hitboxRadius(_hitboxRadius / mapWidth),
      height(_height) {
    float scaleX = 1.0f / mapWidth;
    float scaleY = 1.0f / mapHeight;
    position = sf::Vector2f(_position.x * scaleX, _position.y * scaleY);

    // load its own shadow
    spriteShadow = sf::Sprite(assetShadow);
    sf::Vector2u shadowSize = assetShadow.getSize();
    spriteShadow.setOrigin(shadowSize.x / 2.0f, shadowSize.y);
}

bool WallObject::collisionHasHeightDifference(const float myHeight,
                                              const float otherHeight) {
    // if two players are in different heights they shouldn't collide
    return fabsf(myHeight - otherHeight) > 2.5f;
}

bool WallObject::defaultSolveCollision(CollisionData &data,
                                       const sf::Vector2f &otherSpeed,
                                       const sf::Vector2f &otherPos,
                                       const sf::Vector2f &myPos,
                                       const float distance2) {
    float otherSpeedModule =
        sqrtf(otherSpeed.x * otherSpeed.x + otherSpeed.y * otherSpeed.y) +
        0.05f;
    sf::Vector2f momentum =
        (otherPos - myPos) * otherSpeedModule / (35.0f * sqrtf(distance2));
    data = CollisionData(std::move(momentum), 0.4f);
    return true;
}

bool WallObject::solveCollision(CollisionData &data,
                                const sf::Vector2f &otherSpeed,
                                const sf::Vector2f &otherPos, const float,
                                const float otherHeight, const bool,
                                const float distance2) {
    if (WallObject::collisionHasHeightDifference(height, otherHeight)) {
        return false;
    }
    return WallObject::defaultSolveCollision(data, otherSpeed, otherPos,
                                             position, distance2);
}