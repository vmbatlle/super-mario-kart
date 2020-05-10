#pragma once

#include <SFML/Graphics.hpp>

#include "states/statebase.h"

class StateRacePause : public State {
   private:
    static const sf::Time FADE_TIME;
    const sf::Texture backgroundTexture;
    bool hasPopped = false;
    bool once = true;

    enum class MenuState : int { YES, NO, FADE_OUT };
    MenuState currentState;
    sf::Time fadeTime;

   public:
    StateRacePause(Game& game, const sf::RenderTexture& backgroundRender)
        : State(game),
          backgroundTexture(backgroundRender.getTexture()),
          currentState(MenuState::NO) {}

    void handleEvent(const sf::Event& event) override;
    bool fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "RacePause"; }
};