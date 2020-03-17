#pragma once

class Game;

#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <cmath>
#include <memory>
#include <stack>
#include "map/questionpanel.h"
#include "states/race.h"
#include "states/start.h"
#include "states/statebase.h"

class Game {
   private:
    sf::RenderWindow window;  // game rendering canvas
    int framerate;            // needed to know # of physics updates per second
    bool gameEnded;           // force close the game (e.g. exit/X button)

    int tryPop;
    std::stack<StatePtr> stateStack;
    StatePtr getCurrentState() const;

    void handleEvents(const StatePtr& currentState);
    void handleTryPop();

   public:
    Game(const int _wx, const int _wy, const int _framerate = 60);
    // main game loop until game closed event
    void run();

    void pushState(const StatePtr& statePtr);
    void popState();

    const sf::RenderWindow& getWindow() const;
};