#define _USE_MATH_DEFINES
#include "racestart.h"

#include "map/map.h"

const sf::Time StateRaceStart::ANIMATION_FORWARD_TIME = sf::seconds(8.75f);
const sf::Time StateRaceStart::ANIMATION_TURN_TIME = sf::seconds(10.0f);
const float StateRaceStart::PROB_HIT_BY_CC[(int)CCOption::__COUNT] = {
    0.95, 0.975, 1.0};

void StateRaceStart::asyncLoad() {
    // assumes that course (map) has finished loading
    Map::loadAI();
    asyncLoadFinished = true;
}

void StateRaceStart::init(const sf::Vector2f& _playerPosition) {
    currentTime = sf::Time::Zero;
    accTime = sf::Time::Zero;
    playerPosition = sf::Vector2f(_playerPosition.x / MAP_ASSETS_WIDTH,
                                  _playerPosition.y / MAP_ASSETS_HEIGHT);
    pseudoPlayer = DriverPtr(new Driver(
        "assets/drivers/invisible.png",
        playerPosition + sf::Vector2f(0.0f, ANIMATION_FORWARD_DISTANCE * -1.0f),
        M_PI * -0.5f, MAP_TILES_WIDTH, MAP_TILES_HEIGHT,
        DriverControlType::DISABLED, VehicleProperties::ACCELERATION,
        MenuPlayer(1)));
    pseudoPlayer->setPositionAndReset(playerPosition);

    asyncLoadFinished = false;
    fadingMusic = false;
    loadingThread = std::thread(&StateRaceStart::asyncLoad, this);
}

void StateRaceStart::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ACCEPT, event) &&
        currentTime < sf::seconds(10.0f)) {
        currentTime = sf::seconds(10.0f);
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

bool StateRaceStart::update(const sf::Time& deltaTime) {
    currentTime += deltaTime;
    pushedPauseThisFrame = false;

    Lakitu::update(deltaTime);
    if (currentTime < sf::seconds(2)) {
        float d = currentTime / sf::seconds(2);
        float angle = M_PI_2 * -1.0f * d;
        pseudoPlayer->position =
            sf::Vector2f(0.8f, 0.8f) +
            sf::Vector2f(sinf(angle) * -1.0f, cosf(angle)) * 0.10f;
        pseudoPlayer->posAngle = angle;
    } else if (currentTime < sf::seconds(4)) {
        float d = (currentTime - sf::seconds(2)) / sf::seconds(2);
        pseudoPlayer->position = sf::Vector2f(0.225f, 0.475f) +
                                 sf::Vector2f(0.0f, 0.1f) * (1.0f - d);
        pseudoPlayer->posAngle = M_PI + (M_PI / 8.0f - M_PI_4 * d);
    } else if (currentTime < sf::seconds(6)) {
        float d = (currentTime - sf::seconds(4)) / sf::seconds(2);
        pseudoPlayer->position =
            sf::Vector2f(0.3f, 0.4f) + sf::Vector2f(0.05f, 0.05f) * d;
        pseudoPlayer->posAngle = M_PI_4 * (1.0f - d);
    } else if (currentTime < sf::seconds(8.75)) {
        float d = (currentTime - sf::seconds(6)) / sf::seconds(2.75);
        pseudoPlayer->position =
            playerPosition +
            sf::Vector2f(0.0f, ANIMATION_FORWARD_DISTANCE * -1.0f) * (1.0f - d);
        pseudoPlayer->posAngle = M_PI * 0.5f;
    } else {
        pseudoPlayer->position = playerPosition;
        pseudoPlayer->posAngle = M_PI * -0.5f;
        if (currentTime < sf::seconds(10)) {
            float d = (currentTime - ANIMATION_FORWARD_TIME) /
                      (ANIMATION_TURN_TIME - ANIMATION_FORWARD_TIME);
            pseudoPlayer->posAngle = M_PI * (0.5f - d);
        } else if (Lakitu::isSleeping() && asyncLoadFinished) {
            Lakitu::showStart();
            fadingMusic = true;
        }
        if (fadingMusic) {
            Audio::fadeOut(Music::CIRCUIT_ANIMATION_START, deltaTime);
        }
        if (Lakitu::hasStarted()) {
            if (asyncLoadFinished) {
                loadingThread.join();
                Audio::playEngines(true);
                Audio::play(Music::CIRCUIT_NORMAL);
                for (auto& driver : drivers) {
                    if (driver == player) {
                        if (accTime < sf::seconds(1.5f)) {
                            driver->speedForward = pseudoPlayer->speedForward;
                        } else {
                            driver->applyHit();
                        }
                    } else {
                        float speedPercent = ((75 + rand() % 25) / 100.0f);
                        float turnPercent = ((rand() % 15) / 100.0f);

                        if (speedPercent <= PROB_HIT_BY_CC[(int)ccOption]) {
                            driver->speedForward =
                                speedPercent *
                                driver->vehicle->maxNormalLinearSpeed;
                            driver->speedTurn =
                                turnPercent *
                                driver->vehicle->maxTurningAngularSpeed;
                        } else {
                            driver->applyHit();
                        }
                    }
                }
                game.popState();
            }
        }
    }
    pseudoPlayer->updateSpeed(deltaTime);
    Audio::updateListener(pseudoPlayer->position, pseudoPlayer->posAngle,
                          pseudoPlayer->height);
    Audio::updateEngine(player->position, player->height,
                        pseudoPlayer->speedForward, 0.0f);
    if (Input::held(Key::ACCELERATE)) {
        accTime += deltaTime;
    } else {
        accTime = sf::Time::Zero;
    }
    return true;
}

void StateRaceStart::draw(sf::RenderTarget& window) {
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
        sf::Sprite miniDriver =
            driver->animator.getMinimapSprite(M_PI * -0.5f, scale);
        sf::Vector2f mapPosition = Map::mapCoordinates(driver->position);
        miniDriver.setOrigin(miniDriver.getLocalBounds().width / 2.0f,
                             miniDriver.getLocalBounds().height * 0.9f);
        miniDriver.setPosition(mapPosition.x * windowSize.x,
                               mapPosition.y * windowSize.y);
        miniDriver.scale(0.5f, 0.5f);
        window.draw(miniDriver);
    }

    Lakitu::draw(window);

    Gui::draw(window);

    // informative text
    if (currentTime < sf::seconds(10.0f)) {
        sf::Vector2f position(0.04f, 0.41f);
        if (currentTime > sf::seconds(8.0f)) {
            position.x -= (currentTime - sf::seconds(8.0f)) / sf::seconds(2.0f);
        }
        TextUtils::write(
            window, CIRCUIT_DISPLAY_NAMES[(unsigned int)selectedCircuit],
            sf::Vector2f(position.x * windowSize.x, position.y * windowSize.y),
            windowSize.x / 256.0f, Color::MenuPrimaryOnFocus);

        TextUtils::write(
            window,
            Input::getKeyCodeName(Input::get(Key::ACCEPT)) + " to skip >>",
            sf::Vector2f(position.x * windowSize.x,
                         position.y * 1.12 * windowSize.y),
            (windowSize.x / 256.0f) * 0.5, Color::MenuPrimary);
    }
}