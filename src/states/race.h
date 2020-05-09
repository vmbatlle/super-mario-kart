#pragma once

#include <cmath>
#include <iostream>
#include <string>

#include "ai/gradientdescent.h"
#include "entities/collisionhashmap.h"
#include "entities/driver.h"
#include "entities/item.h"
#include "entities/lakitu.h"
#include "gui/gui.h"
#include "map/enums.h"
#include "map/map.h"
#include "states/statebase.h"
#include "states/racepause.h"

class StateRace : public State {
   private:
    const DriverPtr player;
    DriverArray drivers;
    DriverArray miniDrivers;
    RaceRankingArray& positions;

    bool pushedPauseThisFrame = false;
    bool raceFinished = false;
    bool driftPressed = false;

   public:
    static sf::Time currentTime;

    StateRace(Game& game, const DriverPtr& _player,
              const DriverArray& _drivers, RaceRankingArray& _positions)
        : State(game),
          player(_player),
          drivers(_drivers),
          miniDrivers(_drivers),
          positions(_positions) {
        init();
    }

    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    void init();

    inline std::string string() const override { return "Race"; }
};