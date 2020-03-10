#pragma once

#include <cmath>
#include "game.h"
#include "driver.h"
#include "states/statebase.h"

class StateMode7Test : public State {
   private:
    float posX, posY, posAngle;  // 3 grados de libertad en 2D
    float fovHalf;               // player's field of view
    float clipNear, clipFar;     // view frustrum's near and far planes distance

    float speedForward, speedTurn;
    sf::Image assetImageBottom, assetImageTop;

    bool leftPressed, rightPressed, forwardPressed;

    Driver player;

   public:
    StateMode7Test(Game& game) : State(game) { init(); }
    void init();
    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};