#include "collisionhashmap.h"

CollisionHashMap CollisionHashMap::instance;

int CollisionHashMap::hash(const sf::Vector2f &pos) {
    int hashX = pos.x * NUM_DIVISIONS;
    int hashY = pos.y * NUM_DIVISIONS;
    return hashY * NUM_DIVISIONS + hashX;
}