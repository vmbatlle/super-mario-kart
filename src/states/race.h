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

class StateRace : public State {
   private:
    const DriverPtr player;
    std::vector<DriverPtr> drivers;
    PlayerArray& positions;

    std::array<std::pair<Driver*, int>, (int)MenuPlayer::__COUNT> ranking;

    // TODO llevar el rank por la variable "positions"? (mezclar con points?)
    int rank[(int)MenuPlayer::__COUNT];
    int points[(int)MenuPlayer::__COUNT];

    std::vector<bool> playerCps;
    int playerPassedCps;

   public:
    static sf::Time currentTime;

    StateRace(Game& game, const DriverPtr& _player,
              const std::vector<DriverPtr>& _drivers, PlayerArray& _positions)
        : State(game),
          player(_player),
          drivers(_drivers),
          positions(_positions),
          playerPassedCps(0) {
        init();
    }

    void usePowerUps(bool front, DriverPtr p);

    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    void init();
    void checkpointUpdate();
};