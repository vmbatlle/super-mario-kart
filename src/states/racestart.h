#pragma once

#include <SFML/Graphics.hpp>
#include <thread>

#include "entities/lakitu.h"
#include "states/racemanager.h"
#include "states/statebase.h"

class StateRaceStart : public State {
   private:
    static const sf::Time ANIMATION_FORWARD_TIME;
    static constexpr const float ANIMATION_FORWARD_DISTANCE = 0.2f;
    static const sf::Time ANIMATION_TURN_TIME;
    static const float PROB_HIT_BY_CC[(int)CCOption::__COUNT];
    sf::Time currentTime;
    sf::Time accTime;

    sf::Vector2f playerPosition;
    DriverPtr pseudoPlayer;  // used for animation positioning
    DriverArray drivers;
    DriverPtr player;
    RaceCircuit selectedCircuit;
    CCOption ccOption;

    std::thread loadingThread;
    bool asyncLoadFinished;
    bool fadingMusic;
    bool pushedPauseThisFrame = false;

    void asyncLoad();

   public:
    StateRaceStart(Game& game, const DriverPtr& _player,
                   const DriverArray& _drivers,
                   const sf::Vector2f& _playerPosition,
                   const RaceCircuit _selectedCircuit, CCOption _ccOption)
        : State(game),
          drivers(_drivers),
          player(_player),
          selectedCircuit(_selectedCircuit),
          ccOption(_ccOption) {
        init(_playerPosition);
    }

    ~StateRaceStart() {
        if (loadingThread.joinable()) {
            loadingThread.join();
        }
    }

    void init(const sf::Vector2f& _playerPosition);
    void handleEvent(const sf::Event& event) override;
    bool update(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "RaceStart"; }
};