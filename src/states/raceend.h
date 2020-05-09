#pragma once

#include <SFML/Graphics.hpp>
#include <thread>
#include "states/race.h"
#include "states/statebase.h"

class StateRaceEnd : public State {
   private:
    static const sf::Time ANIMATION_TURN_TIME;
    static const sf::Time ANIMATION_TOTAL_TIME;
    sf::Time timeExecutingState;

    DriverPtr pseudoPlayer;
    const DriverPtr player;
    DriverArray drivers;
    const MenuPlayer selectedPlayer;
    const RaceRankingArray& positions;

   public:
    StateRaceEnd(Game& game, const DriverPtr& _player,
                 const DriverArray& _drivers, const MenuPlayer _selectedPlayer,
                 const RaceRankingArray& _positions)
        : State(game),
          player(_player),
          drivers(_drivers),
          selectedPlayer(_selectedPlayer),
          positions(_positions) {
        init();
    }
    void init();
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "RaceEnd"; }
};