#include "initload.h"

sf::Texture StateInitLoad::shadowTexture;

const sf::Time StateInitLoad::DING_TIME = sf::seconds(0.75f);
const sf::Time StateInitLoad::END_TIME = sf::seconds(2.0f);

void StateInitLoad::loadAllGameTextures() {
    // Floor objects
    Zipper::loadAssets("assets/objects/floor/misc.png",
                       sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(16, 16)));
    QuestionPanel::loadAssets(
        "assets/objects/floor/question_panel.png",
        sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(16, 16)),
        sf::IntRect(sf::Vector2i(0, 16), sf::Vector2i(16, 16)));
    OilSlick::loadAssets(
        "assets/objects/floor/misc.png",
        sf::IntRect(sf::Vector2i(0, 16), sf::Vector2i(16, 16)));
    Coin::loadAssets("assets/objects/floor/misc.png",
                     sf::IntRect(sf::Vector2i(0, 32), sf::Vector2i(8, 8)));
    RampHorizontal::loadAssets(
        "assets/objects/floor/misc.png",
        sf::IntRect(sf::Vector2i(0, 40), sf::Vector2i(8, 8)));
    RampVertical::loadAssets(
        "assets/objects/floor/misc.png",
        sf::IntRect(sf::Vector2i(32, 40), sf::Vector2i(8, 8)));

    // Wall objects
    WallObject::loadAssets("assets/misc/shadow.png");
    Podium::loadAssets("assets/misc/congratulations.png",
                       sf::IntRect(417, 153, 104, 30));
    FloatingFish::loadAssets(
        "assets/misc/congratulations.png", sf::IntRect(2, 51, 90, 95),
        sf::IntRect(2, 148, 93, 95), sf::IntRect(508, 51, 90, 95),
        sf::IntRect(505, 148, 93, 95));
    Pipe::loadAssets("assets/objects/wall/misc.png",
                     sf::IntRect(sf::Vector2i(2, 53), sf::Vector2i(24, 32)),
                     sf::IntRect(sf::Vector2i(158, 53), sf::Vector2i(24, 32)));
    Thwomp::loadAssets(
        "assets/objects/wall/misc.png",
        sf::IntRect(sf::Vector2i(2, 20), sf::Vector2i(24, 32)),
        sf::IntRect(sf::Vector2i(158, 20), sf::Vector2i(24, 32)));

    Banana::loadAssets("assets/objects/wall/misc.png",
                       sf::IntRect(sf::Vector2i(2, 129), sf::Vector2i(16, 16)));

    GreenShell::loadAssets(
        "assets/objects/wall/misc.png",
        sf::IntRect(sf::Vector2i(83, 129), sf::Vector2i(16, 16)));

    RedShell::loadAssets(
        "assets/objects/wall/misc.png",
        sf::IntRect(sf::Vector2i(172, 129), sf::Vector2i(16, 16)));

    EffectCoin::loadAssets("assets/misc/coin.png", sf::IntRect(0, 0, 16, 16),
                           sf::IntRect(16, 0, 16, 16),
                           sf::IntRect(32, 0, 16, 16));
    EffectDrown::loadAssets(
        "assets/misc/particles.png", sf::IntRect(96, 61, 8, 16),
        sf::IntRect(105, 53, 24, 24), sf::IntRect(130, 61, 8, 16),
        sf::IntRect(79, 61, 16, 8), sf::IntRect(79, 52, 16, 8));
    EffectSparkles::loadAssets(
        "assets/misc/particles.png", sf::IntRect(1, 85, 24, 16),
        sf::IntRect(26, 85, 24, 16), sf::IntRect(51, 85, 24, 16),
        sf::IntRect(76, 85, 24, 16));

    // Audio/music assets
    Audio::loadAll();

    // Other menu assets
    EndRanks::loadAssets("assets/gui/ranking.png", sf::IntRect(1, 1, 14, 16),
                         sf::IntRect(18, 1, 14, 16), sf::IntRect(35, 1, 14, 16),
                         17, sf::IntRect(52, 1, 8, 16),
                         sf::IntRect(52, 18, 8, 16), sf::IntRect(52, 35, 8, 16),
                         9);

    TextUtils::loadAssets("assets/gui/letters.png",
                          "assets/gui/letters_alpha.png", sf::Vector2i(1, 1),
                          sf::Vector2i(1, 32));

    StateStart::loadBackgroundAssets("assets/menu/start/background.png",
                                     sf::IntRect(246, 16, 512, 224),
                                     sf::IntRect(6, 16, 234, 76));

    StatePlayerSelection::loadAssets(
        "assets/gui/player_selection.png", sf::IntRect(0, 0, 256, 224),
        sf::IntRect(281, 146, 38, 35), sf::IntRect(272, 24, 256, 16),
        sf::IntRect(352, 57, 16, 8), sf::IntRect(376, 48, 34, 10),
        sf::IntRect(376, 59, 34, 10));

    StateGPStandings::loadAssets("assets/menu/start/background.png",
                                 sf::IntRect(764, 16, 512, 256),
                                 sf::IntRect(887, 301, 256, 224));

    StateCongratulations::loadAssets(
        "assets/misc/congratulations.png", sf::IntRect(410, 2, 141, 14), 15,
        sf::IntRect(553, 2, 10, 15), sf::IntRect(564, 2, 12, 15),
        sf::IntRect(577, 2, 12, 15), 16, sf::IntRect(418, 192, 16, 16),
        sf::IntRect(418, 209, 16, 16), sf::IntRect(418, 227, 16, 16), 20,
        sf::IntRect(65, 2, 62, 22));
}

void StateInitLoad::init() {
    currentTime = sf::Time::Zero;

    nintendoLogoTexture.loadFromFile("assets/gui/nintendo_logo.png");

    audioDingId = Audio::loadDing();
    StateInitLoad::loadAllGameTextures();
}

bool StateInitLoad::update(const sf::Time& deltaTime) {
    currentTime += deltaTime;
    if (!dingPlayed) {
        Audio::play(audioDingId);
        dingPlayed = true;
    } else if (currentTime >= END_TIME) {
        game.pushState(StatePtr(new StateStart(game)));
    }

    return true;
}

void StateInitLoad::draw(sf::RenderTarget& window) {
    window.clear(sf::Color::Black);
    if (currentTime < END_TIME) {
        sf::Sprite nintendoLogo(nintendoLogoTexture);
        float scale =
            window.getSize().x / (float)nintendoLogoTexture.getSize().x;
        nintendoLogo.scale(scale, scale);
        if (currentTime >= DING_TIME) {
            // reduce opacity
            float pct = (currentTime - DING_TIME) / (END_TIME - DING_TIME);
            int opacity = std::max(255 * (1.00f - pct), 0.0f);
            nintendoLogo.setColor(sf::Color(255, 255, 255, opacity));
        }
        window.draw(nintendoLogo);
    }
}
