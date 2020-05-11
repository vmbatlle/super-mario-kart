#pragma once

#include <SFML/Graphics.hpp>
#include <thread>

#include "input/input.h"
#include "states/statebase.h"

class StatePlayerSelection : public State {
   private:
    static sf::Texture assetBackground, assetPlayerBackground, assetMarquee;
    static sf::Texture asset1P, asset1POkQ, asset1POk;  // Q = question mark (?)
    bool hasPopped = false;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roiBackground,
                           const sf::IntRect &roiPlayerBackground,
                           const sf::IntRect &roiMarquee,
                           const sf::IntRect &roi1P,
                           const sf::IntRect &roi1POkQ,
                           const sf::IntRect &roi1POk);

   private:
    static const sf::Time FADE_TOTAL_TIME;
    sf::Time fadeCurrentTime;

    enum class SelectionState : int {
        FADE_IN_INTRO,
        NO_SELECTION,
        FADE_OUT_CANCEL,
        AWAIT_CONFIRMATION,
        SELECTED,
    };
    SelectionState currentState;
    unsigned int framesSinceOrigin;  // requires fixedupdate to be consistent

    MenuPlayer &selectedPlayer;
    std::array<float, (int)MenuPlayer::__COUNT> angles;
    std::vector<DriverAnimator> animators;

    static constexpr const float BACKGROUND_WIDTH = 256.0f;
    static constexpr const float BACKGROUND_HEIGHT = 224.0f;
    static constexpr const float MARQUEE_SPACE_WIDTH = 80.0f;
    static const std::array<sf::Vector2f, (int)MenuPlayer::__COUNT>
        PLAYER_CELL_ORIGINS;
    static const sf::Vector2f REL_TICK;  // relative to PLAYER_CELL_ORIGINS
    static const sf::Vector2f REL_BACKGROUND;
    static const sf::Vector2f REL_PLAYER;
    static const sf::Vector2f ABS_MARQUEE;  // relative to (0, 0)
    static const sf::Vector2f ABS_CONFIRM;

   public:
    StatePlayerSelection(Game &game, MenuPlayer &_selectedPlayer)
        : State(game), selectedPlayer(_selectedPlayer) {
        init();
    }
    void init();
    void handleEvent(const sf::Event &event) override;
    bool fixedUpdate(const sf::Time &deltaTime) override;
    void draw(sf::RenderTarget &window) override;

    inline std::string string() const override { return "PlayerSelection"; }
};