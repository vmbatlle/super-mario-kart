#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>

#include "entities/driver.h"
#include "entities/podium.h"
#include "gui/gui.h"
#include "map/enums.h"
#include "map/map.h"
#include "states/racemanager.h"
#include "states/statebase.h"

class StateCongratulations : public State {
   private:
    const RaceCircuit circuit;
    const MenuPlayer player;
    DriverArray orderedDrivers;
    bool hasPopped = false;

   public:
    // x y height
    static const std::array<sf::Vector3f, 3> PODIUM_DISPLACEMENTS;
    static const sf::Time TIME_FADE, TIME_ANIMATION, TIME_WAIT;
    static const sf::Vector2f CAMERA_DISPLACEMENT;
    static constexpr const float BACKGROUND_WIDTH = 256.0f;
    static constexpr const float BACKGROUND_HEIGHT = 224.0f;

    DriverPtr pseudoPlayer;  // invisible player to move the camera
    unsigned int playerRankedPosition;

    sf::Vector2f targetCameraPosition;
    sf::Time currentTime;

    StateCongratulations(Game& game, const RaceCircuit _circuit,
                         const MenuPlayer _player,
                         const GrandPrixRankingArray& standings)
        : State(game), circuit(_circuit), player(_player) {
        init(standings);
    }

    void init(const GrandPrixRankingArray& standings);
    bool fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "Congratulations"; }
};