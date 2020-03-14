#pragma once

#include <cmath>
#include <iostream>
#include "entities/driver.h"
#include "map/map.h"
#include "states/statebase.h"
#include "entities/lakitu.h"

class StateRace : public State {
   private:
    const std::shared_ptr<Driver> player;
    Lakitu lakitu;

   public:
    StateRace(Game& game, const std::shared_ptr<Driver>& _player)
        : State(game),
        player(_player) {}

    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};