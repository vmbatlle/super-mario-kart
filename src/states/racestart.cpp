#include "racestart.h"

#include "map/map.h"

const sf::Time StateRaceStart::ANIMATION_FORWARD_TIME = sf::seconds(8.75f);
const sf::Time StateRaceStart::ANIMATION_TURN_TIME = sf::seconds(10.0f);

void StateRaceStart::asyncLoad() {
    // assumes that course (map) has finished loading
    Map::loadAI();
    asyncLoadFinished = true;
}

void StateRaceStart::init(const sf::Vector2f& _playerPosition) {
    currentTime = sf::Time::Zero;
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
    loadingThread = std::thread(&StateRaceStart::asyncLoad, this);
}

void StateRaceStart::update(const sf::Time& deltaTime) {
    currentTime += deltaTime;
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
        pseudoPlayer->position =
            sf::Vector2f(0.225f, 0.475f) + sf::Vector2f(0.0f, 0.1f) * (1.0f - d);
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
            Audio::stopMusic();
            Audio::play(SFX::CIRCUIT_LAKITU_SEMAPHORE);
            Lakitu::showStart();
        }
        if (Lakitu::hasStarted()) {
            if (asyncLoadFinished) {
                loadingThread.join();
                Audio::stopMusic();
                Audio::play(Music::CIRCUIT_NORMAL);
                for (auto& driver : drivers) {
                    if (driver == player) {
                        driver->speedForward = pseudoPlayer->speedForward;
                    } else {
                        driver->speedForward =
                            ((75 + rand() % 25) / 100.0f) *
                            driver->vehicle->maxNormalLinearSpeed;
                    }
                }
                game.popState();
            }
        }
    }
    pseudoPlayer->updateSpeed(deltaTime);
    Audio::updateEngine(pseudoPlayer->position, 0.0f,
                        pseudoPlayer->speedForward, 0.0f);
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
}