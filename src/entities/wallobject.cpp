#include "wallobject.h"

WallObject::WallObject(const sf::Vector2f &_position, const float _visualRadius,
                       const float _hitboxRadius, const float _height,
                       const int mapWidth, const int mapHeight)
    : visualRadius(_visualRadius / mapWidth),
      hitboxRadius(_hitboxRadius / mapWidth),
      height(_height) {
    float scaleX = 1.0f / mapWidth;
    float scaleY = 1.0f / mapHeight;
    position = sf::Vector2f(_position.x * scaleX, _position.y * scaleY);
}

bool WallObject::defaultSolveCollision(CollisionData &data,
                                       const sf::Vector2f &otherSpeed,
                                       const sf::Vector2f &otherPos,
                                       const sf::Vector2f &myPos,
                                       const float distance2) {
    float speedModule =
        sqrtf(otherSpeed.x * otherSpeed.x + otherSpeed.y * otherSpeed.y);
    sf::Vector2f momentum =
        (otherPos - myPos) * speedModule / (35.0f * sqrtf(distance2));
    data = CollisionData(std::move(momentum), 0.4f);
    return true;
}

bool WallObject::solveCollision(CollisionData &data,
                                const sf::Vector2f &otherSpeed,
                                const sf::Vector2f &otherPos, const float,
                                const bool, const float distance2) {
    return WallObject::defaultSolveCollision(data, otherSpeed, otherPos,
                                             position, distance2);
}