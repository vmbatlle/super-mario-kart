#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include "game.h"
#include "states/statebase.h"

class StateMode7Test : public State {
   private:
    static constexpr int ASSETS_WIDTH = 1024;
    static constexpr int ASSETS_HEIGHT = 1024;
    static constexpr int TILE_SIZE = 8;
    static constexpr int TILES_WIDTH = ASSETS_WIDTH / TILE_SIZE;
    static constexpr int TILES_HEIGHT = ASSETS_HEIGHT / TILE_SIZE;
    static constexpr float START_POS_X = 143.0f / ASSETS_WIDTH;
    static constexpr float START_POS_Y = 543.0f / ASSETS_HEIGHT;
    static constexpr float START_POS_ANGLE = -M_PI_2;
    static constexpr float CAM_2_PLAYER_DST = 56.0f;

    float playerX, playerY;      // Player position (reuses posAngle)
    float posX, posY, posAngle;  // 3 grados de libertad en 2D
    float fovHalf;               // player's field of view
    float clipNear, clipFar;     // view frustrum's near and far planes distance

    float speedForward, speedTurn;
    sf::Image assetImageBottom, assetImageTop;
    enum class Land : uint8_t {
        TRACK,
        BLOCK,
        OUTER
    };
    Land assetLand[128][128];

   public:
    StateMode7Test(Game& game) : State(game) { init(); }
    void init();
    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};