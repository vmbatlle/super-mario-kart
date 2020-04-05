#include "initload.h"

const sf::Time StateInitLoad::DING_TIME = sf::seconds(1.0f);
const sf::Time StateInitLoad::END_TIME = sf::seconds(2.0f);

#include "entities/driver.h"
#include "entities/vehicleproperties.h"
#include "states/race.h"

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

    // TODO this shouldnt go here, move all this loading to another state (maybe race start)
    // Circuit loading
    Map::loadCourse("assets/circuit/donut_plains_1");
    
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
        // game.popState();


    // Player loading based on circuit
    sf::Vector2f posPlayer = Map::getPlayerInitialPosition(1);
    DriverPtr player = DriverPtr(new Driver(
        "assets/drivers/yoshi.png",
        // sf::Vector2f(143.0f / MAP_ASSETS_HEIGHT,
        // 543.0f / MAP_ASSETS_WIDTH), M_PI_2 * -1.0f));
        sf::Vector2f(posPlayer.x, posPlayer.y), M_PI_2 * -1.0f,
        MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::PLAYER,
        VehicleProperties::BALANCED));

        // TODO this shouldnt be hardcoded here, it's just a test
        const char *players[7] = {
            "assets/drivers/bowser.png",
            "assets/drivers/dk.png",
            "assets/drivers/koopa.png",
            "assets/drivers/luigi.png",
            "assets/drivers/mario.png",
            "assets/drivers/peach.png",
            "assets/drivers/toad.png"
        };
        const VehicleProperties *properties[7] = {
            &VehicleProperties::HEAVY,
            &VehicleProperties::HEAVY,
            &VehicleProperties::HANDLING,
            &VehicleProperties::BALANCED,
            &VehicleProperties::BALANCED,
            &VehicleProperties::ACCELERATION,
            &VehicleProperties::HANDLING,
        };
        std::vector<DriverPtr> drivers = {player};
        for (int pos = 2; pos <= 8; pos++) {
            posPlayer = Map::getPlayerInitialPosition(pos);
            DriverPtr ai = DriverPtr(new Driver(
                players[pos - 2],
                sf::Vector2f(posPlayer.x, posPlayer.y), M_PI_2 * -1.0f,
                MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::AI_GRADIENT,
                *properties[pos - 2]));
            drivers.push_back(ai);
        }
        game.pushState(StatePtr(new StateRace(game, player, drivers)));
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

