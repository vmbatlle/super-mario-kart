#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "entities/collisiondata.h"
#include "entities/driver.h"
#include "entities/wallobject.h"
#include "map/map.h"

class CollisionHashMap {
   private:
    static constexpr const int NUM_DIVISIONS = 8;
    static CollisionHashMap instance;
    template <typename T>
    using CollisionMap = std::array<std::set<T>, NUM_DIVISIONS * NUM_DIVISIONS>;
    CollisionMap<WallObject *> staticMap;
    using DynamicTuple = std::tuple<WallObject *, float, float>;
    CollisionMap<DynamicTuple> dynamicMap;

    static int hash(const sf::Vector2f &pos);
    static inline void insertStatic(const sf::Vector2f &pos,
                                    const WallObjectPtr &object) {
        instance.staticMap[hash(pos)].insert(object.get());
    }
    static inline void insertDynamic(const sf::Vector2f &pos,
                                     const DynamicTuple &data) {
        instance.dynamicMap[hash(pos)].insert(data);
    }

    CollisionHashMap() : staticMap(), dynamicMap() {}

   public:
    static void registerStatic(const WallObjectPtr &object) {
        sf::Vector2f position = object->position;
        float radius = object->hitboxRadius;
        insertStatic(position + sf::Vector2f(radius, 0.0f), object);
        insertStatic(position + sf::Vector2f(radius * -1.0f, 0.0f), object);
        insertStatic(position + sf::Vector2f(0.0f, radius), object);
        insertStatic(position + sf::Vector2f(0.0f, radius * -1.0f), object);
    }

    // objects w/o inertia (e.g. pipes, shells, bananas...)
    static void registerDynamic(const WallObjectPtr &object) {
        sf::Vector2f position = object->position;
        float radius = object->hitboxRadius;
        auto tuple = std::make_tuple((WallObject *)object.get(), 0.0f, 0.0f);
        insertDynamic(position + sf::Vector2f(radius, 0.0f), tuple);
        insertDynamic(position + sf::Vector2f(radius * -1.0f, 0.0f), tuple);
        insertDynamic(position + sf::Vector2f(0.0f, radius), tuple);
        insertDynamic(position + sf::Vector2f(0.0f, radius * -1.0f), tuple);
    }

    // objects with inertia (drivers)
    static void registerDynamic(const DriverPtr &object) {
        sf::Vector2f position = object->position;
        float radius = object->hitboxRadius;
        sf::Vector2f speed =
            object->speedForward *
            sf::Vector2f(cosf(object->posAngle), sinf(object->posAngle));
        auto tuple =
            std::make_tuple((WallObject *)object.get(), speed.x, speed.y);
        insertDynamic(position + sf::Vector2f(radius, 0.0f), tuple);
        insertDynamic(position + sf::Vector2f(radius * -1.0f, 0.0f), tuple);
        insertDynamic(position + sf::Vector2f(0.0f, radius), tuple);
        insertDynamic(position + sf::Vector2f(0.0f, radius * -1.0f), tuple);
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
        for (const auto &tuple : instance.dynamicMap[hash(objPos)]) {
            auto candidate = std::get<0>(tuple);
            if (candidate != object.get()) {
                float sum = objRadius + candidate->hitboxRadius;
                float d2 = dist2(candidate);
                if (d2 > sum * sum) {
                    continue;
                }
                sf::Vector2f candSpeed(std::get<1>(tuple), std::get<2>(tuple));
                if (candidate->solveCollision(
                        data, objSpeed, objPos, object->vehicle->weight,
                        object->height, object->isImmune(), d2)) {
                    return true;
                }
            }
        }
        for (const auto &candidate : instance.staticMap[hash(objPos)]) {
            if (candidate != object.get()) {
                float sum = objRadius + candidate->hitboxRadius;
                float d2 = dist2(candidate);
                if (d2 < sum * sum &&
                    candidate->solveCollision(
                        data, objSpeed, objPos, object->vehicle->weight,
                        object->height, object->isImmune(), d2)) {
                    return true;
                }
            }
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