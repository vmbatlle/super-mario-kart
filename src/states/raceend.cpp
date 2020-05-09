#include "raceend.h"

#include "entities/lakitu.h"
#include "map/map.h"

const sf::Time StateRaceEnd::ANIMATION_TURN_TIME = sf::seconds(1.5f);
const sf::Time StateRaceEnd::ANIMATION_TOTAL_TIME = sf::seconds(10.0f);

void StateRaceEnd::init() {
    currentTime = sf::Time::Zero;
    pseudoPlayer = DriverPtr(new Driver(
        "assets/drivers/invisible.png", sf::Vector2f(0.0f, 0.0f), 0.0f,
        MAP_TILES_WIDTH, MAP_TILES_HEIGHT, DriverControlType::DISABLED,
        VehicleProperties::GODMODE, MenuPlayer(1)));
}

void StateRaceEnd::fixedUpdate(const sf::Time& deltaTime) {
    currentTime += deltaTime;

    // Map object updates
    Map::updateObjects(deltaTime);
    for (DriverPtr& driver : drivers) {
        // Player position updates
        driver->update(deltaTime);
    }
    float turnPct = std::fminf(currentTime / ANIMATION_TURN_TIME, 1.0f);
    pseudoPlayer->position = player->position;
    pseudoPlayer->posAngle = player->posAngle - turnPct * M_PI;

    // Collisions aren't handled on raceend state

    Lakitu::update(deltaTime);

    if (currentTime > ANIMATION_TOTAL_TIME) {
        Lakitu::showUntil(0, deltaTime);
        game.popState();
    }
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

    //Lakitu shadow
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
}