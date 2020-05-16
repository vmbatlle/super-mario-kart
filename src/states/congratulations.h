#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>

#include "entities/driver.h"
#include "entities/floatingfish.h"
#include "entities/podium.h"
#include "gui/gui.h"
#include "map/enums.h"
#include "map/map.h"
#include "states/racemanager.h"
#include "states/statebase.h"

class StateCongratulations : public State {
   private:
    static constexpr const unsigned int NUM_GRATS = 8;
    static std::array<sf::Texture, NUM_GRATS> assetCongratulations, assetOne,
        assetTwo, assetThree;
    static std::array<sf::Texture, 3> assetSmallTrohpies, assetBigTrophies,
        assetMarioTrophies;
    static sf::Texture assetRibbon;

    static constexpr const float BACKGROUND_WIDTH = 256.0f;
    static constexpr const float BACKGROUND_HEIGHT = 224.0f;
    static const sf::Vector2f ABS_GRATS, ABS_RIBBON, ABS_NUMBER1, ABS_NUMBER23;
    static const sf::Vector2f ABS_TROPHY;
    static const sf::Vector2f ABS_TEXT0, REL_TEXTDY;

    const RaceCircuit circuit;
    const RaceMode mode;
    const CCOption ccOption;
    const MenuPlayer player;
    DriverArray orderedDrivers;
    unsigned int framesSinceOrigin;
    bool hasPopped = false;

   public:
    // x y height
    static const std::array<sf::Vector3f, 3> PODIUM_DISPLACEMENTS;
    static const sf::Time TIME_FADE, TIME_ANIMATION, TIME_WAIT;
    static const sf::Time TIME_TROPHY_INITIAL, TIME_TROPHY_MOVEMENT, TIME_TROPHY_FADE;
    static const sf::Vector2f CAMERA_DISPLACEMENT;

    static void loadAssets(
        const std::string &assetName, const sf::IntRect &roiCongrats0,
        const unsigned int congratulationsDY, const sf::IntRect &roiOnes0,
        const sf::IntRect &roiTwos0, const sf::IntRect &roiThrees0,
        const unsigned int numbersDY, const sf::IntRect &roiSmallTrophy0,
        const sf::IntRect &roiBigTrophy0, const sf::IntRect &roiMarioTrophy0,
        const unsigned int trohpiesDX, const sf::IntRect &roiRibbon);

    DriverPtr pseudoPlayer;  // invisible player to move the camera
    unsigned int playerRankedPosition;

    sf::Vector2f targetCameraPosition;
    sf::Time currentTime;

    StateCongratulations(Game &game, const RaceCircuit _circuit,
                         const RaceMode _mode, const CCOption _ccOption,
                         const MenuPlayer _player,
                         const GrandPrixRankingArray &standings)
        : State(game),
          circuit(_circuit),
          mode(_mode),
          ccOption(_ccOption),
          player(_player) {
        init(standings);
    }

    void init(const GrandPrixRankingArray &standings);
    bool fixedUpdate(const sf::Time &deltaTime) override;
    void draw(sf::RenderTarget &window) override;

    inline std::string string() const override { return "Congratulations"; }
};