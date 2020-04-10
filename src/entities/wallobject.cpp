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

bool WallObject::solveCollision(CollisionData &data,
                                const sf::Vector2f &otherSpeed,
                                const sf::Vector2f &otherPos, const float,
                                const float distance2) {
    float speedModule =
        sqrtf(otherSpeed.x * otherSpeed.x + otherSpeed.y * otherSpeed.y);
    sf::Vector2f momentum =
        (otherPos - position) * speedModule / (30.0f * sqrtf(distance2));
    data = CollisionData(std::move(momentum), 0.3f);
    return true;
}