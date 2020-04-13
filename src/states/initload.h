#pragma once

#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include "audio/audio.h"
#include "gui/textutils.h"
#include "entities/banana.h"
#include "states/playerselection.h"
#include "states/start.h"
#include "states/statebase.h"

class StateInitLoad : public State {
   private:
    static const sf::Time DING_TIME;
    static const sf::Time END_TIME;

    sf::Texture nintendoLogoTexture;

    sf::Time currentTime;
    SFX audioDingId;

    bool dingPlayed;

    void loadAllGameTextures();

   public:
    StateInitLoad(Game& game) : State(game) { init(); }
    void init();
    void update(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;
};