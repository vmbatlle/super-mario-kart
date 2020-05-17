#define _USE_MATH_DEFINES
#include <cmath>

#include "entities/lakitu.h"
#include "map/map.h"
#include "raceend.h"

const sf::Time StateRaceEnd::ANIMATION_TURN_TIME = sf::seconds(3.5f);
const sf::Time StateRaceEnd::ANIMATION_ZOOM_OUT_TIME = sf::seconds(6.0f);
const sf::Time StateRaceEnd::ANIMATION_TOTAL_TIME = sf::seconds(10.0f);

void StateRaceEnd::init() {
    timeExecutingState = sf::Time::Zero;
    pseudoPlayer = DriverPtr(new Driver(
        "assets/drivers/invisible.png", sf::Vector2f(0.0f, 0.0f), 0.0f,
        MAP_TILES_WIDTH, MAP_TILES_HEIGHT, DriverControlType::DISABLED,
        VehicleProperties::GODMODE, MenuPlayer(1)));
}

void StateRaceEnd::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ACCEPT, event) &&
        timeExecutingState < ANIMATION_TOTAL_TIME) {
        timeExecutingState = ANIMATION_TOTAL_TIME;
    }
    if (Input::pressed(Key::PAUSE, event) && !pushedPauseThisFrame) {
        pushedPauseThisFrame = true;
        // call draw and store so we can draw it over the screen
        sf::RenderTexture render;
        sf::Vector2u windowSize = game.getWindow().getSize();
        render.create(windowSize.x, windowSize.y);
        fixedUpdate(sf::Time::Zero);
        draw(render);
        game.pushState(StatePtr(new StateRacePause(game, render)));
    }
}

bool StateRaceEnd::fixedUpdate(const sf::Time& deltaTime) {
    timeExecutingState += deltaTime;
    StateRace::currentTime += deltaTime;
    pushedPauseThisFrame = false;

    // Map object updates
    Map::updateObjects(deltaTime);
    for (unsigned int i = 0; i < drivers.size(); i++) {
        DriverPtr& driver = drivers[i];
        // Player position updates
        driver->update(deltaTime);
        Audio::updateEngine(i, driver->position, driver->height,
                            driver->speedForward, driver->speedTurn);
    }
    Audio::updateListener(player->position, player->posAngle, player->height);
    float turnPct = std::fminf(timeExecutingState / ANIMATION_TURN_TIME, 1.0f);
    float animTurnAngle = turnPct * M_PI * -1.0f;
    float zoomOutPct =
        std::fminf(timeExecutingState / ANIMATION_ZOOM_OUT_TIME, 1.0f);
    pseudoPlayer->position =
        player->position +
        sf::Vector2f(cosf(player->posAngle), sinf(player->posAngle)) *
            ZOOM_OUT_DISTANCE * zoomOutPct;
    pseudoPlayer->posAngle = player->posAngle - animTurnAngle;

    // Ranking updates - last gradient contains
    auto hasntFinishedBegin = positions.begin();
    // don't sort drivers that have already finished the circuit
    while ((*hasntFinishedBegin)->getLaps() > NUM_LAPS_IN_CIRCUIT &&
           hasntFinishedBegin < positions.end()) {
        ++hasntFinishedBegin;
    }
    std::sort(hasntFinishedBegin, positions.end(),
              [](const Driver* lhs, const Driver* rhs) {
                  // returns true if player A is ahead of B
                  if (lhs->getLaps() == rhs->getLaps()) {
                      return lhs->getLastGradient() < rhs->getLastGradient();
                  } else {
                      return lhs->getLaps() > rhs->getLaps();
                  }
              });

    // Collisions aren't handled on raceend state

    Lakitu::update(deltaTime);
    EndRanks::update(deltaTime);

    if (timeExecutingState > ANIMATION_TOTAL_TIME && !hasPopped) {
        hasPopped = true;
        Lakitu::showUntil(0, deltaTime);
        game.popState();
    }
    Audio::updateListener(player->position, player->posAngle, player->height);
    return true;
}

void StateRaceEnd::draw(sf::RenderTarget& window) {
    // scale
    static constexpr const float NORMAL_WIDTH = 512.0f;
    const float scale = window.getSize().x / NORMAL_WIDTH;

    // Get textures from map
    sf::Texture tSkyBack, tSkyFront, tCircuit, tMap;
    Map::skyTextures(pseudoPlayer, tSkyBack, tSkyFront);
    Map::circuitTexture(pseudoPlayer, tCircuit);
    Map::mapTexture(tMap);

    // Create sprites and scale them accordingly
    sf::Sprite skyBack(tSkyBack), skyFront(tSkyFront), circuit(tCircuit),
        map(tMap);
    sf::Vector2u windowSize = game.getWindow().getSize();
    float backFactor = windowSize.x / (float)tSkyBack.getSize().x;
    float frontFactor = windowSize.x / (float)tSkyFront.getSize().x;
    skyBack.setScale(backFactor, backFactor);
    skyFront.setScale(frontFactor, frontFactor);

    // Sort them correctly in Y position and draw
    float currentHeight = 0;
    skyBack.setPosition(0.0f, currentHeight);
    skyFront.setPosition(0.0f, currentHeight);
    window.draw(skyBack);
    window.draw(skyFront);
    currentHeight += windowSize.y * Map::SKY_HEIGHT_PCT;
    circuit.setPosition(0.0f, currentHeight);
    window.draw(circuit);

    // Lakitu shadow
    Lakitu::drawShadow(window);

    // Circuit objects (must be before minimap)
    std::vector<std::pair<float, sf::Sprite*>> wallObjects;
    Map::getWallDrawables(window, pseudoPlayer, scale, wallObjects);
    Map::getItemDrawables(window, pseudoPlayer, scale, wallObjects);
    Map::getDriverDrawables(window, pseudoPlayer, drivers, scale, wallObjects);
    std::sort(wallObjects.begin(), wallObjects.end(),
              [](const std::pair<float, sf::Sprite*>& lhs,
                 const std::pair<float, sf::Sprite*>& rhs) {
                  return lhs.first > rhs.first;
              });
    for (const auto& pair : wallObjects) {
        window.draw(*pair.second);
    }

    // Minimap
    currentHeight += windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
    map.setPosition(0.0f, currentHeight);
    window.draw(map);

    // Minimap drivers
    std::sort(drivers.begin(), drivers.end(),
              [](const DriverPtr& lhs, const DriverPtr& rhs) {
                  return lhs->position.y < rhs->position.y;
              });
    for (const DriverPtr& driver : drivers) {
        sf::Sprite miniDriver = driver->animator.getMinimapSprite(
            driver->posAngle + driver->speedTurn * 0.2f, scale);
        sf::Vector2f mapPosition = Map::mapCoordinates(driver->position);
        miniDriver.setOrigin(miniDriver.getLocalBounds().width / 2.0f,
                             miniDriver.getLocalBounds().height * 0.9f);
        miniDriver.setPosition(mapPosition.x * windowSize.x,
                               mapPosition.y * windowSize.y);
        miniDriver.scale(0.5f, 0.5f);
        window.draw(miniDriver);
    }

    // Lakitu
    Lakitu::draw(window);

    // Gui
    Gui::draw(window);

    // end ranks after lakitu
    EndRanks::draw(window);
}