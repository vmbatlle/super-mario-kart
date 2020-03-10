#pragma once

#include <cmath>
#include "game.h"
#include "states/statebase.h"

class StateStart : public State {
   private:
    sf::Image background;
    sf::Texture backgroundDay, backgroundNight, logo;

    float backgroundSpeed;
    float backgroundAspectRatio;
    float backgroundPos;

    bool upPressed, downPressed, leftPressed, rightPressed;

   public:
    StateStart(Game& game) : State(game) { init(); }
    void init();
    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};