#pragma once

class Game;

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <stack>

#include "ai/gradientdescent.h"
#include "entities/enums.h"
#include "entities/pipe.h"
#include "entities/thwomp.h"
#include "entities/vehicleproperties.h"
#include "map/coin.h"
#include "map/oilslick.h"
#include "map/questionpanel.h"
#include "map/ramphorizontal.h"
#include "map/rampvertical.h"
#include "map/zipper.h"
#include "states/statebase.h"

class Game {
   private:
    static const int WINDOW_STYLE = sf::Style::Titlebar | sf::Style::Close;
    const int baseWidth, baseHeight;

    sf::RenderWindow window;  // game rendering canvas
    int framerate;            // needed to know # of physics updates per second
    bool gameEnded;           // force close the game (e.g. exit/X button)

    int tryPop;
    std::stack<StatePtr> stateStack;
    StatePtr getCurrentState() const;

    void handleEvents(const StatePtr& currentState);
    void handleTryPop();

   public:
    Game(const int _bx, const int _by, const int _framerate = 60);
    // main game loop until game closed event
    void run();

    void pushState(const StatePtr& statePtr);
    void popStatesUntil(unsigned int i);
    void popState();

    const sf::RenderWindow& getWindow() const;
    void getCurrentResolution(unsigned int& width, unsigned int& height);
    void updateResolution();
};