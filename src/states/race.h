#pragma once

#include <cmath>
#include <iostream>
#include "entities/driver.h"
#include "map/map.h"
#include "states/statebase.h"
#include "entities/lakitu.h"

class StateRace : public State {
   private:
    const DriverPtr player;

    Lakitu lakitu;

    std::vector<bool> playerCps;
    int playerPassedCps;

   public:
    StateRace(Game& game, const DriverPtr& _player)
        : State(game), player(_player), playerPassedCps(0) { init(); }

    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
    
    void init();
    void checkpointUpdate();
};