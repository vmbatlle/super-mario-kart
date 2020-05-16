#pragma once

#define _USE_MATH_DEFINES
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
#include "states/race.h"
#include "states/racemanager.h"
#include "states/racepause.h"
#include "states/statebase.h"

class StateRaceDemo : public State {
   private:
    static constexpr const float BACKGROUND_WIDTH = 256.0f,
                                 BACKGROUND_HEIGHT = 224.0f;
    static const sf::Vector2f ABS_CREDITS, ABS_NAME0, REL_NAMEDY, ABS_COPY;
    static const sf::Time FADE_TIME;

    static const sf::Time TIME_BETWEEN_ITEM_CHECKS;
    sf::Time nextItemCheck;
    bool autoUseItems;
    bool fixCamera;
    bool firstPersonCamera;

    static const sf::Time SHOW_MESSAGE_TIME, SHOW_MESSAGE_FADE_TIME;
    sf::Time showMessageTime;
    std::string showMessage;

    // race stuff
    DriverArray drivers;
    DriverArray miniDrivers;
    RaceRankingArray positions;

    // camera stuff
    DriverPtr pseudoPlayer;
    unsigned int currentTarget;
    float targetDirection;
    sf::Time nextSwitchTime, nextQPTime;
    static constexpr const float POS_LERP_PCT = 0.03f, ANGLE_LERP_PCT = 0.1f;
    static const sf::Time TIME_BETWEEN_CAMERA_SWITCHES,
        TIME_BETWEEN_QP_REFRESHES;

    // control stuff
    bool raceFinished = false;
    bool fadeFinished = false;
    sf::Time fadeTime;

    void selectRandomTarget();

   public:
    StateRaceDemo(Game& game) : State(game) { init(); }

    void handleEvent(const sf::Event& event) override;
    bool fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    void init();

    inline std::string string() const override { return "RaceDemo"; }
};