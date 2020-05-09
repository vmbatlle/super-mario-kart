#pragma once

#include <SFML/Graphics.hpp>
#include <thread>

#include "entities/lakitu.h"
#include "states/statebase.h"

class StateRaceStart : public State {
   private:
    static const sf::Time ANIMATION_FORWARD_TIME;
    static constexpr const float ANIMATION_FORWARD_DISTANCE = 0.2f;
    static const sf::Time ANIMATION_TURN_TIME;
    sf::Time currentTime;
    sf::Time accTime;

    sf::Vector2f playerPosition;
    DriverPtr pseudoPlayer;  // used for animation positioning
    DriverArray drivers;
    DriverPtr player;

    std::thread loadingThread;
    bool asyncLoadFinished;

    void asyncLoad();

   public:
    StateRaceStart(Game& game, const DriverPtr& _player,
                   const DriverArray& _drivers,
                   const sf::Vector2f& _playerPosition)
        : State(game), drivers(_drivers), player(_player) {
        init(_playerPosition);
    }

    ~StateRaceStart() {
        if (loadingThread.joinable()) {
            loadingThread.join();
        }
    }

    void init(const sf::Vector2f& _playerPosition);
    void update(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "RaceStart"; }
};