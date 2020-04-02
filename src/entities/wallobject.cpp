#include "wallobject.h"

WallObject::WallObject(const sf::Vector2f &_position, const float _radius,
                       const float _height, const int mapWidth,
                       const int mapHeight)
    : radius(_radius / mapWidth), height(_height) {
    float scaleX = 1.0f / mapWidth;
    float scaleY = 1.0f / mapHeight;
    position = sf::Vector2f(_position.x * scaleX, _position.y * scaleY);
}