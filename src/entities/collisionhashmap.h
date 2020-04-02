#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "entities/wallobject.h"
#include "map/map.h"

class CollisionHashMap {
   private:
    static constexpr const int NUM_DIVISIONS = 4;
    static CollisionHashMap instance;
    std::array<std::set<WallObject *>, NUM_DIVISIONS * NUM_DIVISIONS> map;

    static int hash(const sf::Vector2f &pos);
    static inline void insert(const sf::Vector2f &pos,
                              const WallObjectPtr &object) {
        instance.map[hash(pos)].insert(object.get());
    }

    CollisionHashMap() : map() {}

   public:
    static void registerObject(const WallObjectPtr &object) {
        sf::Vector2f position = object->position;
        float radius = object->radius;
        insert(position + sf::Vector2f(radius, 0.0f), object);
        insert(position + sf::Vector2f(radius * -1.0f, 0.0f), object);
        insert(position + sf::Vector2f(0.0f, radius), object);
        insert(position + sf::Vector2f(0.0f, radius * -1.0f), object);
    }

    static void getCollisions(const WallObjectPtr &object,
                              std::vector<WallObject *> &collisions) {
        collisions.clear();
        sf::Vector2f objPos = object->position;
        float objRadius = object->radius;
        for (const auto &candidate : instance.map[hash(objPos)]) {
            if (candidate != object.get()) {
                sf::Vector2f candPos = candidate->position;
                float candRadius = candidate->radius;
                float dx = objPos.x - candPos.x;
                float dy = objPos.y - candPos.y;
                if (dx * dx + dy * dy < objRadius * candRadius) {
                    collisions.push_back(candidate);
                }
            }
        }
    }

    static void reset() {
        for (std::set<WallObject *> &set : instance.map) {
            set.clear();
        }
    }
};