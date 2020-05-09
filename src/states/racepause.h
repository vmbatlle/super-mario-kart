#pragma once

#include <SFML/Graphics.hpp>

#include "states/statebase.h"

class StateRacePause : public State {
   private:
    const sf::Texture backgroundTexture;

    enum class MenuState : int { YES, NO };
    MenuState currentState;

   public:
    StateRacePause(Game& game, const sf::RenderTexture& backgroundRender)
        : State(game),
          backgroundTexture(backgroundRender.getTexture()),
          currentState(MenuState::NO) {}

    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "RacePause"; }
};