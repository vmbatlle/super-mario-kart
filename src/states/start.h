#pragma once

#include <cmath>
#include "audio/audio.h"
#include "game.h"
#include "gui/textutils.h"
#include "states/racemanager.h"
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
        SETTINGS_FADE_IN,   // from menu to settings
        SETTINGS,
        SETTINGS_FADE_OUT,  // from settings to menu
        MENU_FADE_OUT,
        GRAND_PRIX_FADE,  // fade to black, use grand prix
    };
    MenuState currentState;
    sf::Time timeSinceStateChange;

    enum class MenuOption : uint {
        GRAND_PRIX,
        CONTROLS,
        SETTINGS,
        __COUNT,
    };
    uint selectedOption;  // for all menus, not only the first menu
    bool keyChangeRequested, waitingForKeyPress;  // for controls menu

    float backgroundPosition;

    static constexpr const float BACKGROUND_WIDTH = 256.0f,
                                 BACKGROUND_HEIGHT = 224.0f;
    // pixels per second
    static constexpr const float BACKGROUND_PPS = 30.0f;
    static const sf::Vector2f ABS_MENU;
    static const sf::Vector2f ABS_CONTROLS;
    static const sf::Vector2f ABS_SETTINGS;
    static const sf::Vector2f ABS_LOGO;
    static const sf::Time TIME_FADE_TOTAL;

    // menu config
    static const sf::Vector2f MENU_SIZE;
    static const sf::Vector2f ABS_MENU_CENTER;
    static const sf::Time TIME_MENU_TWEEN;
    static const sf::Vector2f REL_TEXT1;  // grand prix
    static const sf::Vector2f REL_TEXT2;  // controls
    static const sf::Vector2f REL_TEXT3;  // settings

    // controls config
    static const sf::Vector2f CONTROLS_SIZE;
    static const sf::Vector2f ABS_CONTROLS_CENTER;
    static const sf::Time TIME_CONTROLS_TWEEN;
    static const sf::Vector2f REL_CONTROL0;   // first element
    static const sf::Vector2f REL_CONTROLDX;  // first to second column
    static const sf::Vector2f REL_CONTROLDY;  // from first to second elements

    // settings config
    static constexpr const uint BASIC_WIDTH = 256, BASIC_HEIGHT = 224;
    static uint resolutionMultiplier;  // game resolution is BASIC_WIDTH *
                                      // multiplier, BASIC_HEIGHT * multiplier;
                                      // valid values are 1, 2, 4 (256x224,
                                      // 512x448, and 1024x896)
    enum class SettingsOption : uint {
        VOLUME_MUSIC,
        VOLUME_SFX,
        RESOLUTION,
        __COUNT,
    };
    static const sf::Vector2f SETTINGS_SIZE;
    static const sf::Vector2f ABS_SETTINGS_CENTER;
    static const sf::Time TIME_SETTINGS_TWEEN;
    static const sf::Vector2f REL_SETTING0;   // first element
    static const sf::Vector2f REL_SETTINGDX;  // first to second column
    static const sf::Vector2f REL_SETTINGDY;  // from first to second elements

   public:
    StateStart(Game& game) : State(game) { init(); }
    void init();
    void handleEvent(const sf::Event& event) override;
    void update(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};