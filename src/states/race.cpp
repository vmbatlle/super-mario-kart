#include "race.h"

// #define DEBUG_POSITION_RANKING  // uncomment to show positions in ranking

const sf::Time StateRace::TIME_BETWEEN_ITEM_CHECKS =
    sf::seconds(1.0f) / (float)Item::UPDATES_PER_SECOND;

const sf::Time StateRace::WAIT_FOR_PC_LAST_PLACE = sf::seconds(5.0f);

void StateRace::init() {
    pushedPauseThisFrame = false;
    StateRace::currentTime = sf::Time::Zero;
    nextItemCheck = sf::Time::Zero;
    waitForPCTime = sf::Time::Zero;
}

void StateRace::handleEvent(const sf::Event& event) {
    // items
    if (Input::pressed(Key::ITEM_FRONT, event) && player->canDrive()) {
        Item::useItem(player, positions, true);
    }
    if (Input::pressed(Key::ITEM_BACK, event) && player->canDrive()) {
        Item::useItem(player, positions, false);
    }

    // drifting
    if (Input::pressed(Key::DRIFT, event) && player->canDrive() &&
        !driftPressed) {
        driftPressed = true;
        player->shortJump();
    }
    if (Input::released(Key::DRIFT, event)) {
        driftPressed = false;
    }

    // pause menu
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

bool StateRace::fixedUpdate(const sf::Time& deltaTime) {
    // don't update if we already popped
    if (raceFinished) {
        return true;
    }

    // update global time
    currentTime += deltaTime;
    pushedPauseThisFrame = false;

    // Map object updates
    for (unsigned int i = 0; i < drivers.size(); i++) {
        DriverPtr& driver = drivers[i];
        // Player position updates
        driver->update(deltaTime);
        Audio::updateEngine(i, driver->position, driver->height,
                            driver->speedForward, driver->speedTurn);
    }
    // check if AI should use its items
    if (currentTime > nextItemCheck) {
        nextItemCheck = currentTime + TIME_BETWEEN_ITEM_CHECKS;
        for (const DriverPtr& driver : drivers) {
            if (driver != player && driver->getPowerUp() != PowerUps::NONE) {
                float r = rand() / (float)RAND_MAX;
                AIItemProb prob = Item::getUseProbability(driver, positions);
                if (r < std::get<0>(prob) / driver->itemProbModifier) {
                    Item::useItem(driver, positions, std::get<1>(prob));
                }
            }
        }
    }
    Map::updateObjects(deltaTime);
    Audio::updateListener(player->position, player->posAngle, player->height);

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
                    if (!driver->isImmune()) {
                        driver->addCoin(-1);
                    }
                    if (driver == player && driver->canDrive() &&
                        driver->speedForward != 0.0f) {
                        Audio::play(SFX::CIRCUIT_COLLISION_PIPE);
                    }
                    Map::addEffectSparkles(driver->position);
                    break;
            }
        }
    }

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
    // find current player and update GUI
    for (unsigned int i = 0; i < positions.size(); i++) {
#ifdef DEBUG_POSITION_RANKING
        // Debug: display ranking with laps and gradient score
        std::cout << i + 1 << ": "
                  << DRIVER_DISPLAY_NAMES[(int)positions[i]->getPj()] << " con "
                  << positions[i]->getLaps() << " y "
                  << positions[i]->getLastGradient() << std::endl;
#endif
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

    EndRanks::update(deltaTime);
    Gui::update(deltaTime);

    // start time counter if all 7 AI finished before the player
    if (waitForPCTime == sf::Time::Zero &&
        positions[positions.size() - 2]->getLaps() > NUM_LAPS_IN_CIRCUIT) {
        waitForPCTime = currentTime + WAIT_FOR_PC_LAST_PLACE;
    }
    // end the race if player has finished or all other AI have finished and the
    // grace time has ended
    if ((player->getLaps() > NUM_LAPS_IN_CIRCUIT ||
         (waitForPCTime != sf::Time::Zero && currentTime > waitForPCTime &&
          player->canDrive() && player->height == 0.0f)) &&
        !raceFinished) {
        raceFinished = true;

        CollisionHashMap::resetStatic();
        CollisionHashMap::resetDynamic();
        Audio::stopSFX();
        Audio::play(SFX::CIRCUIT_GOAL_END);
        Audio::setEnginesVolume(75.0f);
        Gui::stopEffects();

        if (player->getRank() <= 3) {
            Audio::play(SFX::CIRCUIT_END_VICTORY);
            Audio::play(Music::CIRCUIT_PLAYER_WIN);
        } else {
            Audio::play(SFX::CIRCUIT_END_DEFEAT);
            Audio::play(Music::CIRCUIT_PLAYER_LOSE);
        }

        for (const DriverPtr& driver : drivers) {
            driver->endRaceAndReset();
        }

        Lakitu::showFinish();
        Gui::endRace();
        player->controlType = DriverControlType::AI_GRADIENT;
        game.popState();
    }

    return true;
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

    // Lakitu shadow
    Lakitu::drawShadow(window);

    // Circuit objects (must be before minimap)
    std::vector<std::pair<float, sf::Sprite*>> wallObjects;
    Map::getWallDrawables(window, player, scale, wallObjects);
    Map::getItemDrawables(window, player, scale, wallObjects);
    Map::getDriverDrawables(window, player, drivers, scale, wallObjects);
    player->getDrawables(window, scale, wallObjects);
    std::sort(wallObjects.begin(), wallObjects.end(),
              [](const std::pair<float, sf::Sprite*>& lhs,
                 const std::pair<float, sf::Sprite*>& rhs) {
                  return lhs.first > rhs.first;
              });
    for (const auto& pair : wallObjects) {
        window.draw(*pair.second);
    }

    // Particles
    if (player->height == 0.0f &&
        player->speedForward > player->vehicle->maxNormalLinearSpeed / 4) {
        bool small = player->animator.smallTime.asSeconds() > 0 ||
                     player->animator.smashTime.asSeconds() > 0;
        player->animator.drawParticles(window, player->getSprite(), small,
                                       player->position);
    }

    // Minimap
    currentHeight += windowSize.y * Map::CIRCUIT_HEIGHT_PCT;
    map.setPosition(0.0f, currentHeight);
    window.draw(map);

    // Minimap drivers
    std::sort(miniDrivers.begin(), miniDrivers.end(),
              [](const DriverPtr& lhs, const DriverPtr& rhs) {
                  return lhs->position.y < rhs->position.y;
              });
    for (const DriverPtr& driver : miniDrivers) {
        if (!driver->isVisible() && driver != player) continue;
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

    // On top of the circuit, draw lakitu
    Lakitu::draw(window);

    // Draw Gui
    float pctGB = fmaxf(
        0.0f, (waitForPCTime - currentTime) * 255.0f / WAIT_FOR_PC_LAST_PLACE);
    Gui::draw(window, waitForPCTime == sf::Time::Zero
                          ? sf::Color::White
                          : sf::Color(255, pctGB, pctGB));

    // end ranks after lakitu
    EndRanks::draw(window);
}