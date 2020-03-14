#pragma once

#include <cmath>
#include <iostream>
#include "entities/driver.h"
#include "map/map.h"
#include "states/statebase.h"

class StateRace : public State {
   private:
    const DriverPtr player;

   public:
    StateRace(Game& game, const DriverPtr& _player)
        : State(game), player(_player) {}
    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};