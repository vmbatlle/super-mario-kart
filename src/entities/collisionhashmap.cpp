#include "collisionhashmap.h"

CollisionHashMap CollisionHashMap::instance;

int CollisionHashMap::hash(const sf::Vector2f &pos) {
    uint hashX = std::min((uint)(pos.x * NUM_DIVISIONS), NUM_DIVISIONS - 1);
    uint hashY = std::min((uint)(pos.y * NUM_DIVISIONS), NUM_DIVISIONS - 1);
    return hashY * NUM_DIVISIONS + hashX;
}