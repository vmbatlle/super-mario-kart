#pragma once

#include <SFML/Graphics.hpp>

enum class CollisionType : int {
    NO_HIT,
    HIT,
    SMASH,
};

struct CollisionData {
    sf::Vector2f momentum;  // resulting momentum to be added to you
    float speedFactor;      // your speed is reduced by this factor
                            // e.g. speedFactor = 0.5f -> half speed
    CollisionType type;

    CollisionData() {}
    CollisionData(const sf::Vector2f &&_momentum, const float _speedFactor,
                  const CollisionType _type = CollisionType::NO_HIT)
        : momentum(_momentum), speedFactor(_speedFactor), type(_type) {}
};