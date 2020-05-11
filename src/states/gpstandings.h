#pragma once

#include <SFML/Graphics.hpp>

#include "entities/driver.h"
#include "entities/enums.h"
#include "gui/textutils.h"
#include "input/input.h"
#include "states/racemanager.h"
#include "states/statebase.h"

class StateGPStandings : public State {
   private:
    static sf::Texture assetBackground, assetStandings;

   public:
    static void loadAssets(const std::string &assetName,
                           const sf::IntRect &roiBackground,
                           const sf::IntRect &roiStandings);

   private:
    static const sf::Time TIME_PER_PLAYER;  // points are added individually
                                            // time between each point tick
    static const sf::Time TIME_FADE;        // goes to black

    enum class AnimationState {
        FADE_IN,
        INITIAL_RESULTS,  // initial results are shown
        ANIMATING_DOWN,   // scoreboard is cleared and then reordered
        ANIMATING_UP,
        FINAL_RESULTS,  // updated results
        FADE_OUT,
    };
    AnimationState currentState;
    inline void setState(const AnimationState newState) {
        currentState = newState;
        timeSinceStateChange = sf::Time::Zero;
    }

    static const std::array<int, (int)MenuPlayer::__COUNT> POINTS_PER_POSITION;
    inline unsigned int findIndexInRanking(const Driver *driver) const {
        for (unsigned int i = 0; i < standings.size(); i++) {
            if (positions[i] == driver) {
                return i;
            }
        }
        std::cerr << "Error: Unable to find player in race ranking array"
                  << std::endl;
        return -1;
    }
    inline void addPointsToPlayers() {
        for (unsigned int i = 0; i < standings.size(); i++) {
            unsigned int pos = findIndexInRanking(standings[i].first);
            standings[i].second += POINTS_PER_POSITION[pos];
        }
        std::sort(standings.begin(), standings.end(),
                  [](const std::pair<Driver *, int> &lhs,
                     const std::pair<Driver *, int> &rhs) {
                      return lhs.second > rhs.second;
                  });
    }

    float backgroundPosition;
    sf::Time currentTime;  // controls animations
    sf::Time timeSinceStateChange;
    unsigned int framesSinceOrigin;  // ticking of text (color change)
                                     // requires fixedupdate to be consistent

    unsigned int playersShown;  // clear & update animation

    const RaceRankingArray &positions;
    GrandPrixRankingArray &standings;
    const RaceCircuit circuit;
    const MenuPlayer player;  // selected player & circuit

    static constexpr float BACKGROUND_PPS = 30.0f;
    static constexpr float BACKGROUND_WIDTH = 256.0f;
    static constexpr float BACKGROUND_HEIGHT = 224.0f;
    static const sf::Vector2f ABS_ROUND_ID;      // top bar: round xx
    static const sf::Vector2f REL_CIRCUIT_NAME;  // top bar: donut plains 1
    static const sf::Vector2f ABS_SCOREBOARD_0;
    static const sf::Vector2f REL_SCOREBOARD_DX_DRIVER;
    static const sf::Vector2f REL_SCOREBOARD_DX_POINTS;
    static const sf::Vector2f REL_SCOREBOARD_DY;

   public:
    StateGPStandings(Game &game, const RaceRankingArray &_positions,
                     GrandPrixRankingArray &_standings,
                     const RaceCircuit _circuit, const MenuPlayer _player)
        : State(game),
          positions(_positions),
          standings(_standings),
          circuit(_circuit),
          player(_player) {
        init();
    }
    void init();
    void handleEvent(const sf::Event &event) override;
    bool update(const sf::Time &deltaTime) override;
    void draw(sf::RenderTarget &window) override;

    inline std::string string() const override { return "GPStandings"; }
};