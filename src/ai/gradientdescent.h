#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "map/enums.h"

typedef unsigned int uint;

class AIGradientDescent {
    static const int WALL_PENALTY_MAX = 128;
    static const int WALL_PENALTY_FACTOR = 2;
    static const int WALL_PENALTY_ITERS = 4;
    typedef std::array<std::array<int, MAP_TILES_WIDTH>, MAP_TILES_HEIGHT>
        IntMapMatrix;
    static IntMapMatrix gradientMatrix;
    static const std::array<sf::Vector2i, 8> eightNeighbours;

    static int weightLand(const MapLand landType);

   public:
    static void updateGradient(const MapLandMatrix &mapMatrix,
                               const sf::FloatRect &goalLineFloat);

    static sf::Vector2f getNextDirection(const sf::Vector2f &position);
};