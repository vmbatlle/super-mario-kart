#include "collisionhashmap.h"

CollisionHashMap CollisionHashMap::instance;

int CollisionHashMap::hash(const sf::Vector2f &pos) {
    int hashX = pos.x * NUM_DIVISIONS / (float)MAP_ASSETS_WIDTH;
    int hashY = pos.y * NUM_DIVISIONS / (float)MAP_ASSETS_HEIGHT;
    return hashY * NUM_DIVISIONS + hashX;
}