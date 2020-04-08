#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

class StateRaceManager;

#include "entities/driver.h"
#include "entities/vehicleproperties.h"
#include "states/playerselection.h"
#include "states/race.h"
#include "states/racestart.h"
#include "states/statebase.h"

enum class RaceMode : int {
    GRAND_PRIX_1,  // can expand grand prixes here :-)
    VERSUS,
};

const std::array<std::string, 5> CIRCUIT_NAMES = {
    "assets/circuit/donut_plains_1", "assets/circuit/mario_circuit_2",
    "assets/circuit/ghost_valley_1", "assets/circuit/bowser_castle_1",
    "assets/circuit/rainbow_road",
};

class StateRaceManager : public State {
   private:
    RaceMode mode;
    uint currentCircuit;
    uint currentPlayerPosition;      // starts last, goes up hopefully :-)
    std::vector<DriverPtr> drivers;  // order should be the same as MenuPlayer
                                     // not modified by other states

    MenuPlayer selectedPlayer;  // playerselection state modeifies this
    PlayerArray positions;      // other states modify this to update positions

    enum class RaceState : int {
        NO_PLAYER,
        YES_PLAYER,
        RACING,
    };

    RaceState currentState;

    void updatePositions();
    void setPlayer();

   public:
    StateRaceManager(Game &game, const RaceMode _mode)
        : State(game), mode(_mode) {
        init();
    }
    void init();
    void update(const sf::Time &deltaTime) override;

    void setPlayer(const MenuPlayer player);
};