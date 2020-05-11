#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>

#include "audio/audio.h"
#include "entities/banana.h"
#include "entities/effectcoin.h"
#include "entities/effectdrown.h"
#include "entities/effectsparkles.h"
#include "entities/greenshell.h"
#include "entities/podium.h"
#include "entities/redshell.h"
#include "gui/endranks.h"
#include "gui/textutils.h"
#include "states/gpstandings.h"
#include "states/playerselection.h"
#include "states/start.h"
#include "states/statebase.h"

class StateInitLoad : public State {
   private:
    static sf::Texture shadowTexture;  // special case: don't repeat texture
                                       // for all the wallobjects

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
    bool update(const sf::Time& deltaTime) override;
    void draw(sf::RenderTarget& window) override;

    inline std::string string() const override { return "InitLoad"; }
};