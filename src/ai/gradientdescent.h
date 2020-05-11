#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "map/enums.h"

class AIGradientDescent {
    static const int WALL_PENALTY_MAX = 4096;
    static const int WALL_PENALTY_FACTOR = 2;
    static const int WALL_PENALTY_ITERS = 3;
    typedef std::array<std::array<int, MAP_TILES_WIDTH>, MAP_TILES_HEIGHT>
        IntMapMatrix;
    static IntMapMatrix gradientMatrix, positionMatrix;
    static const std::array<sf::Vector2i, 8> eightNeighbours;

    static int weightLand(const MapLand landType);

   public:
    static int GRADIENT_LAP_CHECK;
    static void updateGradient(const MapLandMatrix &mapMatrix,
                               const sf::FloatRect &goalLineFloat);

    static int getPositionValue(const unsigned int col, const unsigned int row);
    static int getPositionValue(const sf::Vector2f &position);

    static sf::Vector2f getNextDirection(const sf::Vector2f &position);
};