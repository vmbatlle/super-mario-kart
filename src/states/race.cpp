#include "race.h"

void StateRace::init() {
    playerCps = std::vector<bool>(Map::numCheckpoints());
    std::fill(playerCps.begin(), playerCps.end(), false);
}

void StateRace::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ITEM_FRONT, event)) {
        // TODO this is example code
        // we can handle item usage by creating an item entity
        // and removing the player's held item
    }
}

void StateRace::fixedUpdate(const sf::Time& deltaTime) {
    player->update(deltaTime);

    checkpointUpdate();

    // Meta condition
    std::cout << playerPassedCps << std::endl;
    if (playerPassedCps >= Map::numCheckpoints() &&
        Map::inGoal(player->position)) {
        player->rounds++;
        playerPassedCps = 0;
        std::fill(playerCps.begin(), playerCps.end(), false);
        lakitu.onScreen = true;
        lakitu.update(deltaTime);
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
    currentHeight += windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
    map.setPosition(0.0f, currentHeight);
    window.draw(map);

    // TODO this shouldnt be constructed here, instead taken as class attribute
    std::vector<DriverPtr> drivers = {player};

    std::vector<DriverPtr> sorted(drivers);
    std::sort(sorted.begin(), sorted.end(),
              [](const DriverPtr& lhs, const DriverPtr& rhs) {
                  return lhs->position.y <
                         rhs->position.y;  // TODO esto igual va al reves (>)
              });
    for (const DriverPtr& driver : sorted) {
        sf::Sprite miniDriver(driver->animator.sprite);
        // TODO interpolate position from minimap points
        miniDriver.setPosition(
            driver->position.x * windowSize.x,
            driver->position.y * windowSize.y * Map::MINIMAP_HEIGHT_PCT);
        float scale = 1.0f / 3.0f;
        miniDriver.scale(scale, scale);
        miniDriver.move(0.0f, currentHeight);
        window.draw(miniDriver);
    }

    player->draw(window);
    if (lakitu.onScreen) {
        lakitu.draw(window);
    }
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