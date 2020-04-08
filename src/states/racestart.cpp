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
        DriverControlType::DISABLED, VehicleProperties::GODMODE));

    asyncLoadFinished = false;
    loadingThread = std::thread(&StateRaceStart::asyncLoad, this);
    Lakitu::setWindowSize(game.getWindow().getSize());
}

void StateRaceStart::update(const sf::Time& deltaTime) {
    currentTime += deltaTime;
    Lakitu::update(deltaTime);
    if (currentTime < sf::seconds(2)) {
        float d = currentTime / sf::seconds(2);
        pseudoPlayer->position =
            sf::Vector2f(0.2, 0.2) +
            sf::Vector2f(ANIMATION_FORWARD_DISTANCE * 1.0f, 0.0f) * (1.0f - d);
        pseudoPlayer->posAngle = 0.0f;
    } else if (currentTime < sf::seconds(4)) {
        float d = (currentTime - sf::seconds(2)) / sf::seconds(2);
        pseudoPlayer->position =
            sf::Vector2f(0.2, 0.8) +
            sf::Vector2f(ANIMATION_FORWARD_DISTANCE * 1.0f, 0.0f) * (1.0f - d);
        pseudoPlayer->posAngle = M_PI;
    } else if (currentTime < sf::seconds(6)) {
        float d = (currentTime- sf::seconds(4)) / sf::seconds(2);
        pseudoPlayer->position =
            sf::Vector2f(0.8, 0.2) +
            sf::Vector2f(0.0f, ANIMATION_FORWARD_DISTANCE * -1.0f) * (1.0f - d);
        pseudoPlayer->posAngle = M_PI * 0.5f;
    } else if (currentTime < sf::seconds(8.75)) {
        float d = (currentTime - sf::seconds(6)) / sf::seconds(2.75);
        pseudoPlayer->position =
            playerPosition +
            sf::Vector2f(0.0f, ANIMATION_FORWARD_DISTANCE * -1.0f) * (1.0f - d);
        pseudoPlayer->posAngle = M_PI * 0.5f;
    } else {
        pseudoPlayer->position = playerPosition;
        if (currentTime < sf::seconds(10)) {
            float d = (currentTime - ANIMATION_FORWARD_TIME) /
                      (ANIMATION_TURN_TIME - ANIMATION_FORWARD_TIME);
            pseudoPlayer->posAngle = M_PI * (0.5f - d);
        } else if (Lakitu::isSleeping() && asyncLoadFinished) {
            Lakitu::showStart();
        } 
        if (Lakitu::hasStarted()){
            pseudoPlayer->posAngle = M_PI * -0.5f;
            if (asyncLoadFinished) {
                loadingThread.join();
                game.popState();
            }
        }
    }
}

void StateRaceStart::draw(sf::RenderTarget& window) {
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
    Map::getWallDrawables(window, pseudoPlayer, wallObjects);
    Map::getDriverDrawables(window, pseudoPlayer, drivers, wallObjects);
    wallObjects.push_back(pseudoPlayer->getDrawable(window));
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
            driver->animator.getMinimapSprite(M_PI * -0.5f);
        sf::Vector2f mapPosition = Map::mapCoordinates(driver->position);
        miniDriver.setPosition(mapPosition.x * windowSize.x,
                               mapPosition.y * windowSize.y +
                                   miniDriver.getLocalBounds().height / 2);
        miniDriver.scale(0.5f, 0.5f);
        // move the driver up a bit so mapPosition corresponds to the bottom
        // center of the sprite
        miniDriver.move(0.0f, miniDriver.getTexture()->getSize().y *
                                  miniDriver.getScale().y * -0.3f);
        window.draw(miniDriver);
    }

    Lakitu::draw(window);

    Gui::draw(window);
}