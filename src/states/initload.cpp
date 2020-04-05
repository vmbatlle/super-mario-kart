#include "initload.h"

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

    // Wall objects
    Pipe::loadAssets("assets/objects/wall/misc.png",
                     sf::IntRect(sf::Vector2i(2, 53), sf::Vector2i(24, 32)),
                     sf::IntRect(sf::Vector2i(158, 53), sf::Vector2i(24, 32)));
    Thwomp::loadAssets(
        "assets/objects/wall/misc.png",
        sf::IntRect(sf::Vector2i(2, 20), sf::Vector2i(24, 32)),
        sf::IntRect(sf::Vector2i(158, 20), sf::Vector2i(24, 32)));

    // Audio/music assets
    Audio::loadAll();

    // Other menu assets
    StateStart::loadBackgroundAssets("assets/menu/start/background.png",
                                     sf::IntRect(246, 16, 512, 224),
                                     sf::IntRect(6, 16, 234, 76));

    StatePlayerSelection::loadAssets(
        "assets/gui/player_selection.png", sf::IntRect(0, 0, 256, 224),
        sf::IntRect(281, 146, 38, 35), sf::IntRect(272, 24, 256, 16),
        sf::IntRect(352, 57, 16, 8), sf::IntRect(376, 48, 34, 10),
        sf::IntRect(376, 59, 34, 10));

    finishedLoading = true;
}

void StateInitLoad::init() {
    currentTime = sf::Time::Zero;

    nintendoLogoTexture.loadFromFile("assets/gui/nintendo_logo.png");

    audioDingId = Audio::loadDing();
    dingPlayed = false;
    finishedLoading = false;
    loadingThread = std::thread(&StateInitLoad::loadAllGameTextures, this);
}

void StateInitLoad::update(const sf::Time& deltaTime) {
    currentTime += deltaTime;
    if (!dingPlayed && currentTime >= DING_TIME) {
        Audio::play(audioDingId);
        dingPlayed = true;
    } else if (currentTime >= END_TIME && finishedLoading) {
        loadingThread.join();
        // TODO temporary fix - move to game start
        game.popState();
    }
}

void StateInitLoad::draw(sf::RenderTarget& window) {
    window.clear(sf::Color::Black);
    sf::Sprite nintendoLogo(nintendoLogoTexture);
    float scale = window.getSize().x / (float)nintendoLogoTexture.getSize().x;
    nintendoLogo.scale(scale, scale);
    if (currentTime >= DING_TIME) {
        // reduce opacity
        float pct = (currentTime - DING_TIME) / (END_TIME - DING_TIME);
        int opacity = std::max(255 * (1.00f - pct), 0.0f);
        nintendoLogo.setColor(sf::Color(255, 255, 255, opacity));
    }
    window.draw(nintendoLogo);
}
