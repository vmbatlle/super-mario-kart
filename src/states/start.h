#pragma once

#include <cmath>
#include "game.h"
#include "states/statebase.h"

class StateStart : public State {
   private:
    static sf::Texture assetBackground, assetLogo;

   public:
    static void loadBackgroundAssets(const std::string& assetName,
                                     const sf::IntRect& roiBackground,
                                     const sf::IntRect& roiLogo);

   private:
    enum class MenuState : int {
        NO_MENUS,
        MENU_FADE_IN,  // from background to menu
        MENU,
        CONTROLS_FADE_IN,  // from menu to controls
        CONTROLS,
        CONTROLS_FADE_OUT,  // from controls to menu
        MENU_FADE_OUT,
    };
    MenuState currentState;
    sf::Time timeSinceStateChange;

    float backgroundPosition;

    static constexpr const float BACKGROUND_WIDTH = 256.0f,
                                 BACKGROUND_HEIGHT = 224.0f;
    // pixels per second
    static constexpr const float BACKGROUND_PPS = 30.0f;
    static const sf::Vector2f ABS_MENU;
    static const sf::Vector2f ABS_LOGO;
    static const sf::Vector2f MENU_SIZE;
    static const sf::Vector2f ABS_MENU_CENTER;

   public:
    StateStart(Game& game) : State(game) { init(); }
    void init();
    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};