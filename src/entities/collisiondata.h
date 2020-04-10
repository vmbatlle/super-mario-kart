#pragma once

#include <SFML/Graphics.hpp>

struct CollisionData {
    sf::Vector2f momentum;    // resulting momentum to be added to you
    float speedFactor;        // your speed is reduced by this factor
                              // e.g. speedFactor = 0.5f -> half speed

    CollisionData() {}
    CollisionData(const sf::Vector2f &&_momentum,
                  const float _speedFactor)
        : momentum(_momentum), speedFactor(_speedFactor) {}
};