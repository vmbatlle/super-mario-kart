#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

class StateRaceManager;

#include "entities/driver.h"
#include "entities/vehicleproperties.h"
#include "states/playerselection.h"
#include "states/race.h"
#include "states/raceend.h"
#include "states/racestart.h"
#include "states/statebase.h"

enum class RaceMode : int {
    GRAND_PRIX_1,  // can expand grand prixes here :-)
    VERSUS,
};

enum class RaceCircuit : int {
    DONUT_PLAINS_1,
    MARIO_CIRCUIT_2,
    GHOST_VALLEY_1,
    BOWSER_CASTLE_1,
    RAINBOW_ROAD,
    __COUNT,
};

const std::array<std::string, 5> CIRCUIT_DISPLAY_NAMES = {
    "donut plains 1",  "mario circuit 2", "ghost valley 1",
    "bowser castle 1", "rainbow road",
};

const std::array<std::string, 5> CIRCUIT_ASSET_NAMES = {
    "assets/circuit/donut_plains_1", "assets/circuit/mario_circuit_2",
    "assets/circuit/ghost_valley_1", "assets/circuit/bowser_castle_1",
    "assets/circuit/rainbow_road",
};

class StateRaceManager : public State {
   private:
    RaceMode mode;
    RaceCircuit currentCircuit;
    float speedMultiplier;       // 50cc/100cc/150cc
    DriverArray drivers;         // order should be the same as MenuPlayer
                                 // not modified by other states

    MenuPlayer selectedPlayer;   // playerselection state modifies this
    RaceRankingArray positions;  // other states modify this to update positions
                                 // in the current race
                                 // index i -> position i+1 in race
                                 // e.g. first element is position 1 aka first
    GrandPrixRankingArray grandPrixRanking;  // driver-ints pairs

    enum class RaceState : int {
        NO_PLAYER,
        YES_PLAYER,
        RACING,
    };

    RaceState currentState;

    void updatePositions();
    void setPlayer();

   public:
    StateRaceManager(Game &game, const RaceMode _mode,
                     const float _speedMultiplier,
                     const RaceCircuit _circuit = RaceCircuit(0))
        : State(game), mode(_mode) {
        init(_speedMultiplier, _circuit);
    }
    void init(const float _speedMultiplier, const RaceCircuit _circuit);
    void update(const sf::Time &deltaTime) override;

    void setPlayer(const MenuPlayer player);
};