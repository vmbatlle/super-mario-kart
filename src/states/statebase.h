#pragma once

#include <memory>
class State;
typedef std::shared_ptr<State> StatePtr;

#include <SFML/Graphics.hpp>
#include "game.h"
#include "input/input.h"

class State {
   protected:
    Game& game;

   public:
    State(Game& _game) : game(_game) {}
    // https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Event.php
    virtual void handleEvent(const sf::Event&) {}
    // update executes once per frame
    virtual bool update(const sf::Time&) { return false; }
    // fixedUpdate executes every fixed time step (physics calculations)
    virtual bool fixedUpdate(const sf::Time&) { return false; }
    virtual void draw(sf::RenderTarget&) {}

    virtual std::string string() const = 0;
};