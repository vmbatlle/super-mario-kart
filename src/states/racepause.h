#pragma once

#include <SFML/Graphics.hpp>
#include "states/statebase.h"

class StateRacePause : public State {
   private:
    const sf::Texture backgroundTexture;

   public:
    StateRacePause(Game& game, const sf::RenderTexture& backgroundRender)
        : State(game), backgroundTexture(backgroundRender.getTexture()) {}

    void handleEvent(const sf::Event& event) override;
    void fixedUpdate(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};