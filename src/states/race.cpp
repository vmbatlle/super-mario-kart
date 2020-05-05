#include "race.h"

void StateRace::init() { StateRace::currentTime = sf::seconds(0); }

void StateRace::handleEvent(const sf::Event& event) {
    // items
    if (Input::pressed(Key::ITEM_FRONT, event) && player->canDrive()) {
        Item::useItem(player, positions, true);
    }
    if (Input::pressed(Key::ITEM_BACK, event) && player->canDrive()) {
        Item::useItem(player, positions, false);
    }

    // drifting
    if (Input::pressed(Key::DRIFT, event) && player->canDrive()) {
        player->shortJump();
    }

    // pause menu
    if (Input::pressed(Key::PAUSE, event)) {
        // call draw and store so we can draw it over the screen
        sf::RenderTexture render;
        sf::Vector2u windowSize = game.getWindow().getSize();
        render.create(windowSize.x, windowSize.y);
        draw(render);
        game.pushState(StatePtr(new StateRacePause(game, render)));
    }
}

void StateRace::fixedUpdate(const sf::Time& deltaTime) {
    // update global time
    currentTime += deltaTime;

    // Map object updates
    for (DriverPtr& driver : drivers) {
        // Player position updates
        driver->update(deltaTime);

        if (driver != player && driver->getPowerUp() != PowerUps::NONE) {
            float r = rand() / (float)RAND_MAX;
            if (r < 0.001) Item::useItem(driver, positions, true);
        }
    }
    Map::updateObjects(deltaTime);

    // Collision updates
    // Register all objects for fast detection
    CollisionHashMap::resetDynamic();
    Map::registerItemObjects();
    for (const DriverPtr& driver : drivers) {
        CollisionHashMap::registerDynamic(driver);
    }

    // Detect collisions with players
    CollisionData data;
    for (const DriverPtr& driver : drivers) {
        if (CollisionHashMap::collide(driver, data)) {
            driver->collisionMomentum = data.momentum;
            driver->speedForward *= data.speedFactor;
            driver->speedTurn *= data.speedFactor;
            switch (data.type) {
                case CollisionType::HIT:
                    driver->applyHit();
                    break;
                case CollisionType::SMASH:
                    driver->applySmash();
                    break;
                default:
                    break;
            }
        }
    }

    // Ranking updates - last gradient contains
    std::sort(positions.begin(), positions.end(),
              [](const Driver* lhs, const Driver* rhs) {
                  // returns true if player A is ahead of B
                  if (lhs->getLaps() == rhs->getLaps()) {
                      return lhs->getLastGradient() < rhs->getLastGradient();
                  } else {
                      return lhs->getLaps() > rhs->getLaps();
                  }
              });
    // find current player and update GUI
    for (uint i = 0; i < positions.size(); i++) {
        // Debug: display ranking with laps and gradient score
        // std::cout << i + 1 << ": "
        //           << DRIVER_DISPLAY_NAMES[(int)positions[i]->getPj()] << "
        //           con "
        //           << positions[i]->getLaps() << " y "
        //           << positions[i]->getLastGradient() << std::endl;
        positions[i]->rank = i;
        if (positions[i] == player.get()) {
            Gui::setRanking(i + 1);
        }
    }

    // UI updates

    Lakitu::update(deltaTime);
    bool hasChanged = FloorObject::applyAllChanges();
    if (hasChanged) {
        Map::updateMinimap();
    }

    Gui::update(deltaTime);

    if (player->getLaps() == 6) {
        Audio::stopSFX();
        Gui::stopEffects();

        for (DriverPtr driver : drivers) {
            driver->endRaceAndReset();
        }
        Lakitu::showFinish();
        player->controlType = DriverControlType::AI_GRADIENT;
        game.popState();
    }
}

void StateRace::draw(sf::RenderTarget& window) {
    // scale
    static constexpr const float NORMAL_WIDTH = 512.0f;
    const float scale = window.getSize().x / NORMAL_WIDTH;

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
    Map::getWallDrawables(window, player, scale, wallObjects);
    Map::getItemDrawables(window, player, scale, wallObjects);
    Map::getDriverDrawables(window, player, drivers, scale, wallObjects);
    auto playerDrawable = player->getDrawable(window, scale);
    wallObjects.push_back(playerDrawable);
    std::sort(wallObjects.begin(), wallObjects.end(),
              [](const std::pair<float, sf::Sprite*>& lhs,
                 const std::pair<float, sf::Sprite*>& rhs) {
                  return lhs.first > rhs.first;
              });
    for (const auto& pair : wallObjects) {
        window.draw(*pair.second);
    }

    // Particles
    if (player->animator.drifting) {
        bool small = player->animator.smallTime.asSeconds() > 0 ||
                     player->animator.smashTime.asSeconds() > 0;
        player->animator.drawParticles(window, playerDrawable.second, small);
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
            driver->posAngle + driver->speedTurn * 0.5f, scale);
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

    // On top of the circuit, draw lakitu
    Lakitu::draw(window);

    // Draw Gui
    Gui::draw(window);
}