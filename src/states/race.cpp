#include "race.h"

void StateRace::init() {
    playerCps = std::vector<bool>(Map::numCheckpoints());
    std::fill(playerCps.begin(), playerCps.end(), false);
    lakitu.setWindowSize(game.getWindow().getSize());
    lakitu.showStart();
    Map::startCourse();
}

void StateRace::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ITEM_FRONT, event)) {
        // TODO this is example code
        // we can handle item usage by creating an item entity
        // and removing the player's held item
    }
}

void StateRace::fixedUpdate(const sf::Time& deltaTime) {
    // Player position updates
    player->update(deltaTime);

    // TODO this shouldnt be constructed here, instead taken as class attribute
    std::vector<DriverPtr> drivers = {player};

    // Now that players are updated, check map/etc
    checkpointUpdate();

    // Goal condition
    if (playerPassedCps >= Map::numCheckpoints() &&
        Map::inGoal(player->position)) {
        player->rounds++;
        playerPassedCps = 0;
        std::fill(playerCps.begin(), playerCps.end(), false);
        lakitu.showLap(2);
    }
    lakitu.update(deltaTime);
    bool hasChanged = FloorObject::applyAllChanges();
    if (hasChanged) {
        Map::updateMinimap();
    }
}

void StateRace::draw(sf::RenderTarget& window) {
    // Get textures from map
    sf::Texture tSkyBack, tSkyFront, tCircuit, tMap;
    Map::skyTextures(player, tSkyBack, tSkyFront);
    Map::circuitTexture(player, tCircuit);
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
    Map::getDrawables(window, player, wallObjects);
    wallObjects.push_back(player->getDrawable(window));
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
    // TODO this shouldnt be constructed here, instead taken as class attribute
    std::vector<DriverPtr> drivers = {player};

    std::vector<DriverPtr> sorted(drivers);
    std::sort(sorted.begin(), sorted.end(),
              [](const DriverPtr& lhs, const DriverPtr& rhs) {
                  return lhs->position.y <
                         rhs->position.y;  // TODO esto igual va al reves (>)
              });
    for (const DriverPtr& driver : sorted) {
        sf::Sprite miniDriver = driver->animator.getMinimapSprite(
            driver->posAngle + driver->speedTurn * 0.5f);
        sf::Vector2f mapPosition = Map::mapCoordinates(driver->position);
        miniDriver.setPosition(mapPosition.x * windowSize.x,
                               mapPosition.y * windowSize.y);
        miniDriver.scale(0.5f, 0.5f);
        // move the driver up a bit so mapPosition corresponds to the bottom
        // center of the sprite
        miniDriver.move(0.0f, miniDriver.getTexture()->getSize().y *
                                  miniDriver.getScale().y * -0.3f);
        window.draw(miniDriver);
    }

    // On top of the circuit, draw lakitu
    lakitu.draw(window);
}

void StateRace::checkpointUpdate() {
    sf::Vector2f ppos = player->position;

    int i = 0;
    for (sf::FloatRect cp : Map::getCheckpoints()) {
        if (!playerCps[i] && cp.contains(ppos)) {
            playerCps[i] = true;
            playerPassedCps++;
        }
    }
}