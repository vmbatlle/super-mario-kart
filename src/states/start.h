#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <map>

#include "settings.h"
#include "audio/audio.h"
#include "game.h"
#include "gui/textutils.h"
#include "map/map.h"
#include "states/racedemo.h"
#include "states/racemanager.h"
#include "states/statebase.h"

typedef std::map<const std::string, std::string> SettingsMap;

class StateStart : public State {
   private:
    // players that go from left to right
    std::vector<sf::Vector2f> driverMovementPercent;
    std::vector<float> driverSpeed;
    std::vector<DriverAnimator> driverAnimators;

    static sf::Texture assetBackground, assetLogo;
    SettingsMap settings;

    std::thread randomMapLoadingThread;
    bool randomMapLoaded = false, randomMapOverwritten = false;
    void asyncLoad();
    void loadRandomMap();

    sf::Texture assetLoadedMap;
    void loadPreview(const RaceCircuit circuit);

   public:
    static void loadBackgroundAssets(const std::string& assetName,
                                     const sf::IntRect& roiBackground,
                                     const sf::IntRect& roiLogo);

   private:
    enum class MenuState : int {
        INTRO_FADE_IN,  // initial state, fade to menu
        NO_MENUS,
        DEMO_FADE,     // from background to demo
        MENU_FADE_IN,  // from background to menu
        MENU,
        CC_FADE_IN,  // from menu to cc selection
        CC,
        CC_FADE_OUT,      // from cc selection to menu
        CIRCUIT_FADE_IN,  // from cc selection to circuit selection
        CIRCUIT,
        CIRCUIT_FADE_OUT,  // from circuit selection to cc selection
        CONTROLS_FADE_IN,  // from menu to controls
        CONTROLS,
        CONTROLS_FADE_OUT,  // from controls to menu
        SETTINGS_FADE_IN,   // from menu to settings
        SETTINGS,
        SETTINGS_FADE_OUT,  // from settings to menu
        MENU_FADE_OUT,
        GAME_FADE,  // fade to black, use selected game mode
        EXIT_CONFIRM,
    };
    MenuState currentState;
    sf::Time timeSinceStateChange;

    enum class MenuOption : unsigned int {
        GRAND_PRIX,
        VERSUS,
        CONTROLS,
        SETTINGS,
        __COUNT,
    };
    unsigned int selectedOption;  // for all menus, not only the first menu
    bool keyChangeRequested, waitingForKeyPress;  // for controls menu

    float backgroundPosition;

    static constexpr const float BACKGROUND_WIDTH = 256.0f,
                                 BACKGROUND_HEIGHT = 224.0f;
    // pixels per second
    static constexpr const float BACKGROUND_PPS = -22.0f;
    static const sf::Vector2f ABS_MENU;
    static const sf::Vector2f ABS_CC;
    static const sf::Vector2f ABS_CIRCUIT;
    static const sf::Vector2f ABS_CONTROLS;
    static const sf::Vector2f ABS_SETTINGS;
    static const sf::Vector2f ABS_LOGO;
    static const sf::Time TIME_FADE_TOTAL;

    // demo fade
    static const sf::Time TIME_DEMO_WAIT;

    // menu config
    static const sf::Vector2f MENU_SIZE;
    static const sf::Vector2f ABS_MENU_CENTER;
    static const sf::Time TIME_MENU_TWEEN;
    static const sf::Vector2f REL_TEXT1;  // grand prix
    static const sf::Vector2f REL_TEXT2;  // versus
    static const sf::Vector2f REL_TEXT3;  // controls
    static const sf::Vector2f REL_TEXT4;  // settings

    // cc
    static const sf::Vector2f CC_SIZE;
    static const sf::Vector2f ABS_CC_CENTER;
    static const sf::Time TIME_CC_TWEEN;
    static const sf::Vector2f REL_CC0;   // first element
    static const sf::Vector2f REL_CCDY;  // from first to second elements

    // circuit selection menu
    static const sf::Vector2f CIRCUIT_SIZE;
    static const sf::Vector2f ABS_CIRCUIT_CENTER;
    static const sf::Time TIME_CIRCUIT_TWEEN;
    static const sf::Vector2f REL_CIRCUIT0;   // first element
    static const sf::Vector2f REL_CIRCUITDY;  // from first to second elements

    // controls config
    static const sf::Vector2f CONTROLS_SIZE;
    static const sf::Vector2f ABS_CONTROLS_CENTER;
    static const sf::Time TIME_CONTROLS_TWEEN;
    static const sf::Vector2f REL_CONTROL0;   // first element
    static const sf::Vector2f REL_CONTROLDX;  // first to second column
    static const sf::Vector2f REL_CONTROLDY;  // from first to second elements

    enum class SettingsOption : unsigned int {
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

    MenuOption selectedMode;      // set in MENU, remembered after cc selected
    RaceCircuit selectedCircuit;  // set in MENU (only for vs), remembered after
                                  // cc selected
    CCOption selectedCC;          // set in MENU, remembered after cc selected

   public:
    StateStart(Game& game) : State(game) { init(); }
    ~StateStart() {
        if (randomMapLoadingThread.joinable()) {
            randomMapLoadingThread.join();
        }
    }
    void init();
    void handleEvent(const sf::Event& event) override;
    bool update(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "Start"; }
};