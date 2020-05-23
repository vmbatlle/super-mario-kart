#include "gradientdescent.h"

#include "map/map.h"

// #define DEBUG_GRADIENT  // prints values

AIGradientDescent::IntMapMatrix AIGradientDescent::gradientMatrix,
    AIGradientDescent::positionMatrix;

int AIGradientDescent::GRADIENT_LAP_CHECK = 0;
int AIGradientDescent::MAX_POSITION_MATRIX = 0;

const std::array<sf::Vector2i, 8> AIGradientDescent::eightNeighbours = {
    sf::Vector2i(0, -1), sf::Vector2i(-1, 0), sf::Vector2i(0, 1),
    sf::Vector2i(1, 0),  sf::Vector2i(1, -1), sf::Vector2i(1, 1),
    sf::Vector2i(-1, 1), sf::Vector2i(-1, -1)};

int AIGradientDescent::weightLand(const MapLand landType) {
    switch (landType) {
        case MapLand::ZIPPER:
        case MapLand::OTHER:  // special floor objects (boxes, coins)
            return 1;
        case MapLand::TRACK:
        case MapLand::SPECIAL_13H:
            return 10;
        case MapLand::RAMP:
        case MapLand::RAMP_HORIZONTAL:
        case MapLand::RAMP_VERTICAL:
            return 50;
        case MapLand::OIL_SLICK:
        case MapLand::SLOW:
            return 100;
        case MapLand::OUTER:
        case MapLand::BLOCK:
            return 500000;
        default:
            std::cerr << "AIGradientDescent::weightLand: Invalid landType ("
                      << (int)landType << ")" << std::endl;
            return 0;
    }
}

void AIGradientDescent::updateGradient(const MapLandMatrix &mapMatrix,
                                       const sf::FloatRect &goalLineFloat) {
    const std::string gradient = Map::getCourse() + "/gradient.txt";
    const std::string position = Map::getCourse() + "/position.txt";

    std::ifstream in(gradient);
    std::ifstream in2(position);
    if (in.good() && in2.good()) {
        for (unsigned int row = 0; row < mapMatrix.size(); row++) {
            for (unsigned int col = 0; col < mapMatrix[0].size(); col++) {
                in >> gradientMatrix[row][col];
            }
        }
        for (unsigned int row = 0; row < mapMatrix.size(); row++) {
            for (unsigned int col = 0; col < mapMatrix[0].size(); col++) {
                in2 >> positionMatrix[row][col];
            }
        }
        in2 >> GRADIENT_LAP_CHECK;
        MAX_POSITION_MATRIX = GRADIENT_LAP_CHECK + 10;
    } else {
        // Initialize map with empty values
        for (auto &row : gradientMatrix) {
            row.fill(-2);
        }
        for (auto &row : positionMatrix) {
            row.fill(-1);
        }

        // Mark walls
        IntMapMatrix wallPenalty;
        std::vector<sf::Vector2i> wallPenaltyFrontier;
        unsigned int numRows = mapMatrix.size();
        unsigned int numCols = mapMatrix[0].size();
        for (unsigned int row = 0; row < numRows; row++) {
            for (unsigned int col = 0; col < numCols; col++) {
                if (mapMatrix[row][col] == MapLand::BLOCK || row == 0 ||
                    row == numRows - 1 || col == 0 || col == numCols - 1) {
                    gradientMatrix[row][col] = -1;
                    wallPenalty[row][col] = WALL_PENALTY_MAX;
                    wallPenaltyFrontier.push_back(sf::Vector2i(col, row));
                } else if (mapMatrix[row][col] == MapLand::SLOW ||
                           mapMatrix[row][col] == MapLand::OUTER) {
                    wallPenalty[row][col] = WALL_PENALTY_MAX;
                    wallPenaltyFrontier.push_back(sf::Vector2i(col, row));
                } else {
                    wallPenalty[row][col] = 0;
                }
            }
        }

        // Add penalties for being close to walls
        int currentPenalty = WALL_PENALTY_MAX;
        for (int pen = 0; pen < WALL_PENALTY_ITERS; pen++) {
            std::vector<sf::Vector2i> nextWallPenaltyFrontier;
            for (const sf::Vector2i &point : wallPenaltyFrontier) {
                int row = point.y;
                int col = point.x;
                int penalty = wallPenalty[row][col];
                if (penalty != currentPenalty) {
                    continue;
                }
                for (const sf::Vector2i &neighbour : eightNeighbours) {
                    // unsigned int to avoid comapring less than 0
                    unsigned int prow = row + neighbour.y;
                    unsigned int pcol = col + neighbour.x;
                    if (prow < numRows && pcol < numCols &&
                        wallPenalty[prow][pcol] == 0) {
                        wallPenalty[prow][pcol] = penalty / WALL_PENALTY_FACTOR;
                        nextWallPenaltyFrontier.push_back(
                            sf::Vector2i(pcol, prow));
                    }
                }
            }
            wallPenaltyFrontier = nextWallPenaltyFrontier;
            currentPenalty /= WALL_PENALTY_FACTOR;
        }

        // Mark goal line as -1 and add frontier
        sf::IntRect goalLineInt(goalLineFloat.left * MAP_TILES_WIDTH,
                                goalLineFloat.top * MAP_TILES_HEIGHT,
                                goalLineFloat.width * MAP_TILES_WIDTH,
                                goalLineFloat.height * MAP_TILES_HEIGHT);
        using WeightTuple = std::tuple<int, int, int, int>;  // x, y, pos, grad
        std::vector<WeightTuple> frontier;
        sf::Vector2i lapCheckPos;
        for (int irow = 0; irow < goalLineInt.height; irow++) {
            for (int icol = 0; icol < goalLineInt.width; icol++) {
                int row = goalLineInt.top + irow;
                int col = goalLineInt.left + icol;
                gradientMatrix[row][col] = 0;
                positionMatrix[row][col] = 0;
                // mark bottom tracks as starting frontier
                if (mapMatrix[row + 1][col] == MapLand::TRACK) {
                    int initialWeight =
                        weightLand(MapLand::TRACK) + wallPenalty[row + 1][col];
                    gradientMatrix[row + 1][col] = initialWeight;
                    positionMatrix[row + 1][col] = 1;
                    frontier.push_back(
                        WeightTuple(col, row + 1, initialWeight, 1));
                }
                if (mapMatrix[row - 1][col] == MapLand::TRACK) {
                    lapCheckPos = sf::Vector2i(col, row - 1);
                }
            }
        }

        // Fill whole matrix
        while (!frontier.empty()) {
            std::vector<WeightTuple> nextFrontier;
            for (const WeightTuple &point : frontier) {
                for (const sf::Vector2i &neighbour : eightNeighbours) {
                    int row = std::get<1>(point);
                    int col = std::get<0>(point);
                    if (mapMatrix[row][col] == MapLand::SPECIAL_13H) {
                        col -= 13;  // special case for mario circuit 2's jump
                    }
                    row += neighbour.y;
                    col += neighbour.x;
                    int weight = std::get<2>(point) +
                                 weightLand(mapMatrix[row][col]) +
                                 wallPenalty[row][col];
                    int position = std::get<3>(point) + 1;
                    // check if its lower than the current best
                    if (gradientMatrix[row][col] == -2 ||
                        gradientMatrix[row][col] > weight) {
                        gradientMatrix[row][col] = weight;
                        positionMatrix[row][col] = position;
                        // update current weight with new found best
                        bool found = false;
                        for (auto &element : nextFrontier) {
                            if (std::get<0>(element) == col &&
                                std::get<1>(element) == row) {
                                std::get<2>(element) = weight;
                                std::get<3>(element) = position;
                                found = true;
                                break;
                            }
                        }
                        // can't update, add new node
                        if (!found) {
                            nextFrontier.push_back(
                                WeightTuple(col, row, weight, position));
                        }
                    }
                }
            }
            frontier = nextFrontier;
        }

        GRADIENT_LAP_CHECK = positionMatrix[lapCheckPos.y][lapCheckPos.x] - 10;
        MAX_POSITION_MATRIX = GRADIENT_LAP_CHECK + 10;
        std::ofstream out(gradient);
        for (unsigned int row = 0; row < mapMatrix.size(); row++) {
            for (unsigned int col = 0; col < mapMatrix[0].size(); col++) {
                out << gradientMatrix[row][col] << " ";
            }
            out << std::endl;
        }
        std::ofstream out2(position);
        for (unsigned int row = 0; row < mapMatrix.size(); row++) {
            for (unsigned int col = 0; col < mapMatrix[0].size(); col++) {
                out2 << positionMatrix[row][col] << " ";
            }
            out2 << std::endl;
        }
        out2 << GRADIENT_LAP_CHECK << std::endl;
    }
}

int AIGradientDescent::getPositionValue(unsigned int col, unsigned int row) {
    row = std::min(row, (unsigned int)MAP_TILES_WIDTH - 1u);
    col = std::min(col, (unsigned int)MAP_TILES_WIDTH - 1u);
    return positionMatrix[row][col];
}

int AIGradientDescent::getPositionValue(const sf::Vector2f &position) {
    return getPositionValue(position.x * MAP_TILES_WIDTH,
                            position.y * MAP_TILES_HEIGHT);
}

sf::Vector2f AIGradientDescent::getNextDirection(const sf::Vector2f &position) {
    int row = position.y * MAP_TILES_HEIGHT;
    int col = position.x * MAP_TILES_WIDTH;
    sf::Vector2i bestDirection(0, 0);
    int bestWeight = gradientMatrix[row][col];
#ifdef DEBUG_GRADIENT
    std::cout << "Position: " << col << ", " << row << ": "
              << gradientMatrix[row][col] << std::endl;
#endif
    // special case: driver is at the finish line
    if (bestWeight == 0) {
        sf::Vector2f up = sf::Vector2f(0.0f, -1.0f / MAP_TILES_HEIGHT);
#ifdef DEBUG_GRADIENT
        std::cout << "Special case: move up " << up.x << " " << up.y
                  << std::endl;
#endif
        return up;
    }
#ifdef DEBUG_GRADIENT
    for (const sf::Vector2i &neighbour : eightNeighbours) {
        std::cout << "Neighbour " << neighbour.x << ", " << neighbour.y << ": "
                  << gradientMatrix[row + neighbour.y][col + neighbour.x]
                  << std::endl;
    }
#endif
    // check eight neighbours
    auto bestIter = eightNeighbours.begin();
    int bestValue = -1;
    while (bestValue == -1) {
        bestValue = gradientMatrix[row + bestIter->y][col + bestIter->x];
        bestIter++;
    }
    auto iter = --bestIter;
    while (iter != eightNeighbours.end()) {
        int candValue = gradientMatrix[row + iter->y][col + iter->x];
        if (bestValue > candValue && candValue != -1 &&
            // if you have the goal below (candValue == 0),
            // you can't go down (iter->y == 1)
            (candValue != 0 || iter->y != 1)) {
            bestValue = candValue;
            bestIter = iter;
        }
        iter++;
    }
#ifdef DEBUG_GRADIENT
    std::cout << "Selected neighbour " << bestIter->x << ", " << bestIter->y
              << ": " << gradientMatrix[row + bestIter->y][col + bestIter->x]
              << std::endl;
#endif
    sf::Vector2f dirScaled(bestIter->x / (float)MAP_TILES_WIDTH,
                           bestIter->y / (float)MAP_TILES_HEIGHT);
    return dirScaled;
}