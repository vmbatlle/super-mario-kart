#pragma once

#include <cmath>
#include "game.h"
#include "gui/textutils.h"
#include "states/statebase.h"
#include "states/racemanager.h"

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
        GRAND_PRIX_FADE,  // fade to black, use grand prix
    };
    MenuState currentState;
    sf::Time timeSinceStateChange;

    uint selectedOption;      // for menus
    bool keyChangeRequested, waitingForKeyPress;  // for controls menu

    float backgroundPosition;

    static constexpr const float BACKGROUND_WIDTH = 256.0f,
                                 BACKGROUND_HEIGHT = 224.0f;
    // pixels per second
    static constexpr const float BACKGROUND_PPS = 30.0f;
    static const sf::Vector2f ABS_MENU;
    static const sf::Vector2f ABS_CONTROLS;
    static const sf::Vector2f ABS_LOGO;
    static const sf::Time TIME_FADE_TOTAL;

    // menu config
    static const sf::Vector2f MENU_SIZE;
    static const sf::Vector2f ABS_MENU_CENTER;
    static const sf::Time TIME_MENU_TWEEN;
    static const sf::Vector2f REL_TEXT1;  // grand prix
    static const sf::Vector2f REL_TEXT2;  // controls

    // controls config
    static const sf::Vector2f CONTROLS_SIZE;
    static const sf::Vector2f ABS_CONTROLS_CENTER;
    static const sf::Time TIME_CONTROLS_TWEEN;
    static const sf::Vector2f REL_CONTROL0;   // first element
    static const sf::Vector2f REL_CONTROLDX;  // first to second column
    static const sf::Vector2f REL_CONTROLDY;  // from first to second elements

   public:
    StateStart(Game& game) : State(game) { init(); }
    void init();
    void handleEvent(const sf::Event& event) override;
    void update(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};