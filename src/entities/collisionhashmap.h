#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "entities/collisiondata.h"
#include "entities/driver.h"
#include "entities/item.h"
#include "entities/wallobject.h"
#include "map/map.h"

class CollisionHashMap {
   private:
    static constexpr const unsigned int NUM_DIVISIONS = 8;
    static CollisionHashMap instance;
    template <typename T>
    using CollisionMap = std::array<std::set<T>, NUM_DIVISIONS * NUM_DIVISIONS>;
    CollisionMap<WallObject *> staticMap, dynamicMap;

    static int hash(const sf::Vector2f &pos);
    static inline void insertStatic(const sf::Vector2f &pos,
                                    const WallObjectPtr &object) {
        instance.staticMap[hash(pos)].insert(object.get());
    }
    static inline void insertDynamic(const sf::Vector2f &pos,
                                     const WallObjectPtr &object) {
        instance.dynamicMap[hash(pos)].insert(object.get());
    }

    CollisionHashMap() : staticMap(), dynamicMap() {}

   public:
    static void registerStatic(const WallObjectPtr &object) {
        sf::Vector2f position = object->position;
        float radius = object->hitboxRadius;
        // 4-neighbours
        insertStatic(position + sf::Vector2f(radius, 0.0f), object);
        insertStatic(position + sf::Vector2f(radius * -1.0f, 0.0f), object);
        insertStatic(position + sf::Vector2f(0.0f, radius), object);
        insertStatic(position + sf::Vector2f(0.0f, radius * -1.0f), object);
        // 8-neighbours
        insertStatic(position + sf::Vector2f(radius, radius), object);
        insertStatic(position + sf::Vector2f(radius * -1.0f, radius), object);
        insertStatic(position + sf::Vector2f(radius * -1.0f, radius * -1.0f),
                     object);
        insertStatic(position + sf::Vector2f(radius, radius * -1.0f), object);
    }

    // objects w/o inertia (e.g. pipes, shells, bananas...)
    static void registerDynamic(const WallObjectPtr &object) {
        sf::Vector2f position = object->position;
        float radius = object->hitboxRadius;
        // 4-neighbours
        insertDynamic(position + sf::Vector2f(radius, 0.0f), object);
        insertDynamic(position + sf::Vector2f(radius * -1.0f, 0.0f), object);
        insertDynamic(position + sf::Vector2f(0.0f, radius), object);
        insertDynamic(position + sf::Vector2f(0.0f, radius * -1.0f), object);
        // 8-neighbours
        insertDynamic(position + sf::Vector2f(radius, radius), object);
        insertDynamic(position + sf::Vector2f(radius * -1.0f, radius), object);
        insertDynamic(position + sf::Vector2f(radius * -1.0f, radius * -1.0f),
                      object);
        insertDynamic(position + sf::Vector2f(radius, radius * -1.0f), object);
    }

    // objects with inertia (drivers)
    static void registerDynamic(const DriverPtr &object) {
        sf::Vector2f position = object->position;
        float radius = object->hitboxRadius;
        insertDynamic(position + sf::Vector2f(radius, 0.0f), object);
        insertDynamic(position + sf::Vector2f(radius * -1.0f, 0.0f), object);
        insertDynamic(position + sf::Vector2f(0.0f, radius), object);
        insertDynamic(position + sf::Vector2f(0.0f, radius * -1.0f), object);
    }

    static bool collide(const DriverPtr &object, CollisionData &data) {
        sf::Vector2f objPos = object->position;
        float objRadius = object->hitboxRadius;
        sf::Vector2f objSpeed =
            object->speedForward *
            sf::Vector2f(cosf(object->posAngle), sinf(object->posAngle));
        const auto dist2 = [&objPos](const WallObject *candidate) {
            sf::Vector2f candPos = candidate->position;
            float dx = objPos.x - candPos.x;
            float dy = objPos.y - candPos.y;
            return dx * dx + dy * dy;
        };
        for (const auto &candidate : instance.dynamicMap[hash(objPos)]) {
            if (candidate == object.get()) {
                continue;
            }
            float sum = objRadius + candidate->hitboxRadius;
            float d2 = dist2(candidate);
            if (d2 > sum * sum) {
                continue;
            }
            if (candidate->solveCollision(
                    data, objSpeed, objPos, object->vehicle->weight,
                    object->height, object->isImmune(), d2)) {
                return true;
            }
        }
        for (const auto &candidate : instance.staticMap[hash(objPos)]) {
            if (candidate == object.get()) {
                continue;
            }
            float sum = objRadius + candidate->hitboxRadius;
            float d2 = dist2(candidate);
            if (d2 < sum * sum &&
                candidate->solveCollision(
                    data, objSpeed, objPos, object->vehicle->weight,
                    object->height, object->isImmune(), d2)) {
                return true;
            }
        }
        return false;
    }

    static bool evade(const Driver *self, const sf::Vector2f &position,
                      float hitboxRadius, sf::Vector2f &evadeVector) {
        const auto dist2 = [&position](const WallObject *candidate) {
            sf::Vector2f candPos = candidate->position;
            float dx = position.x - candPos.x;
            float dy = position.y - candPos.y;
            return dx * dx + dy * dy;
        };
        for (const WallObject *candidate :
             instance.dynamicMap[hash(position)]) {
            if (candidate == self) {
                continue;
            }
            float sum = hitboxRadius + candidate->hitboxRadius;
            float d2 = dist2(candidate);
            if (d2 > sum * sum) {
                continue;
            }
            const Item *itemCandidate = dynamic_cast<const Item *>(candidate);
            if (itemCandidate && !itemCandidate->registersCollisions()) {
                continue;
            }
            CollisionData data;
            evadeVector = position - candidate->position;
            return true;
        }
        for (const auto &candidate : instance.staticMap[hash(position)]) {
            if (candidate == self) {
                continue;
            }
            float sum = hitboxRadius + candidate->hitboxRadius;
            float d2 = dist2(candidate);
            if (d2 > sum * sum) {
                continue;
            }
            CollisionData data;
            evadeVector = position - candidate->position;
            return true;
        }
        return false;
    }

    static void resetStatic() {
        for (auto &set : instance.staticMap) {
            set.clear();
        }
    }

    static void resetDynamic() {
        for (auto &set : instance.dynamicMap) {
            set.clear();
        }
    }
};