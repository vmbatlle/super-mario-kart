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
    static int MAX_POSITION_MATRIX;
    // Rubber banding for AI (if it has enough speed ~ 40%)
    // DIST: [MAX_DISTANCE_BEHIND...0] PJ [0.............MAX_DISTANCE_AHEAD]
    // PROB: [1.0.....MIN_PROB_BEHIND] PJ [MIN_PROB_BEHIND...MIN_PROB_AHEAD]
    // WHERE
    //      DIST: distance of manhattan in map tiles.
    //      PROB: probability of accelrrating on that update.
    static constexpr int MAX_DISTANCE_BEHIND[(int)CCOption::__COUNT] = {100, 100, 75};
    static constexpr float MIN_PROB_BEHIND[(int)CCOption::__COUNT] = {0.6f, 0.7f, 0.75f};
    static constexpr int MAX_DISTANCE_AHEAD[(int)CCOption::__COUNT] = {100, 200, 300};
    static constexpr float MIN_PROB_AHEAD[(int)CCOption::__COUNT] = {0.45f, 0.45f, 0.45f};
    static void updateGradient(const MapLandMatrix &mapMatrix,
                               const sf::FloatRect &goalLineFloat);

    static int getPositionValue(unsigned int col, unsigned int row);
    static int getPositionValue(const sf::Vector2f &position);

    static sf::Vector2f getNextDirection(const sf::Vector2f &position);
};