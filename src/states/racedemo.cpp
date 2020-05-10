#include "racedemo.h"

const sf::Vector2f StateRaceDemo::ABS_CREDITS =
    sf::Vector2f(58.0f / BACKGROUND_WIDTH, 12.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateRaceDemo::ABS_NAME0 =
    sf::Vector2f(52.0f / BACKGROUND_WIDTH, 156.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateRaceDemo::REL_NAMEDY =
    sf::Vector2f(0.0f / BACKGROUND_WIDTH, 12.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateRaceDemo::ABS_COPY =
    sf::Vector2f(48.0f / BACKGROUND_WIDTH, 204.0f / BACKGROUND_HEIGHT);

const sf::Time StateRaceDemo::FADE_TIME = sf::seconds(0.5f);
const sf::Time StateRaceDemo::TIME_BETWEEN_CAMERA_SWITCHES = sf::seconds(8.0f),
               StateRaceDemo::TIME_BETWEEN_QP_REFRESHES = sf::seconds(0.4f);

const sf::Time StateRaceDemo::TIME_BETWEEN_ITEM_CHECKS =
    sf::seconds(1.0f) / (float)Item::UPDATES_PER_SECOND;

void StateRaceDemo::selectRandomTarget() {
    targetDirection = (nextSwitchTime.asMilliseconds() % 360) * M_PI / 180.0f;
    uint lastTarget = currentTarget;
    while (currentTarget == lastTarget) {
        currentTarget = rand() % drivers.size();
    }
    nextSwitchTime = StateRace::currentTime + TIME_BETWEEN_CAMERA_SWITCHES;
}

void StateRaceDemo::init() {
    StateRace::currentTime = sf::Time::Zero;
    nextItemCheck = sf::Time::Zero;
    fadeTime = sf::Time::Zero;
    autoUseItems = true;

    VehicleProperties::setScaleFactor(2.0f, 1.0f);

    // init pseudoplayer
    pseudoPlayer = DriverPtr(
        new Driver("assets/drivers/invisible.png", sf::Vector2f(0.0f, 0.0f),
                   M_PI_2 * -1.0f, MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT,
                   DriverControlType::DISABLED, VehicleProperties::GODMODE,
                   MenuPlayer(0)));

    // init players
    sf::Vector2f pos = Map::getPlayerInitialPosition(1);
    pos = sf::Vector2f(pos.x / MAP_ASSETS_WIDTH, pos.y / MAP_ASSETS_HEIGHT);
    for (uint i = 0; i < drivers.size(); i++) {
        while (rand() % 7 != 0) {
            pos += AIGradientDescent::getNextDirection(pos);
        }
        sf::Vector2f dir = AIGradientDescent::getNextDirection(pos);
        float angle = atan2(dir.y, dir.x);

        DriverPtr driver(new Driver(
            DRIVER_ASSET_NAMES[i].c_str(), sf::Vector2f(0.0f, 0.0f), 0.0f,
            MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::AI_GRADIENT,
            *DRIVER_PROPERTIES[i], MenuPlayer(i)));
        driver->setPositionAndReset(pos, angle);
        drivers[i] = driver;
        miniDrivers[i] = driver;
        positions[i] = driver.get();
    }
}

void StateRaceDemo::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed &&
        event.key.code >= sf::Keyboard::Num1 &&
        event.key.code <= sf::Keyboard::Num7) {
        PowerUps item = PowerUps(event.key.code - sf::Keyboard::Num0);
        for (const DriverPtr& driver : drivers) {
            driver->pickUpPowerUp(item);
            if (autoUseItems) {
                Item::useItem(driver, positions, true);
            }
        }
    } else if (event.type == sf::Event::KeyPressed &&
               event.key.code == sf::Keyboard::Space) {
        selectRandomTarget();
    } else if (event.type == sf::Event::KeyPressed &&
               event.key.code == sf::Keyboard::Tab) {
        autoUseItems = !autoUseItems;
    } else if (!raceFinished && event.type == sf::Event::KeyPressed) {
        raceFinished = true;
        fadeTime = sf::Time::Zero;
    }
}

bool StateRaceDemo::fixedUpdate(const sf::Time& deltaTime) {
    // update global time
    StateRace::currentTime += deltaTime;
    fadeTime += deltaTime;
    if (raceFinished && fadeTime >= FADE_TIME) {
        if (!fadeFinished) {
            fadeFinished = true;
            game.popState();
            game.popState();  // return to initLoad to push another start state
        }
        return true;
    }

    // Map object updates
    for (uint i = 0; i < drivers.size(); i++) {
        // Player position updates
        drivers[i]->update(deltaTime);
    }
    // check if AI should use its items
    if (StateRace::currentTime > nextItemCheck) {
        nextItemCheck = StateRace::currentTime + TIME_BETWEEN_ITEM_CHECKS;
        for (uint i = 0; i < drivers.size(); i++) {
            if (i == currentTarget &&
                drivers[i]->getPowerUp() != PowerUps::NONE) {
                float r = rand() / (float)RAND_MAX;
                AIItemProb prob =
                    Item::getUseProbability(drivers[i], positions);
                if (r < std::get<0>(prob)) {
                    Item::useItem(drivers[i], positions, std::get<1>(prob));
                }
            }
        }
    }
    Map::updateObjects(deltaTime);

    // only for the demo - reactivate them every few seconds
    if (StateRace::currentTime > nextQPTime) {
        nextQPTime = StateRace::currentTime + TIME_BETWEEN_QP_REFRESHES;
        Map::reactivateQuestionPanels();
    }

    // Pseudo-player (camera) update
    if (StateRace::currentTime > nextSwitchTime) {
        // target switch
        selectRandomTarget();
    }
    // camera movement
    sf::Vector2f targetPosition =
        drivers[currentTarget]->position +
        sf::Vector2f(cosf(targetDirection), sinf(targetDirection)) * 0.05f;
    pseudoPlayer->position +=
        (targetPosition - pseudoPlayer->position) * POS_LERP_PCT;
    sf::Vector2f d = drivers[currentTarget]->position - pseudoPlayer->position;
    float targetAngle = atan2(d.y, d.x);
    // more smooth angle transition (don't do 0-2pi range)
    while (fabsf(targetAngle - pseudoPlayer->posAngle) >
           fabsf(targetAngle + 2.0f * M_PI - pseudoPlayer->posAngle)) {
        targetAngle += 2.0f * M_PI;
    }
    while (fabsf(targetAngle - pseudoPlayer->posAngle) >
           fabsf(targetAngle - pseudoPlayer->posAngle - 2.0f * M_PI)) {
        targetAngle -= 2.0f * M_PI;
    }
    pseudoPlayer->posAngle +=
        (targetAngle - pseudoPlayer->posAngle) * ANGLE_LERP_PCT;

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
    for (uint i = 0; i < positions.size(); i++) {
        positions[i]->rank = i;
    }

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
                    Map::addEffectSparkles(driver->position);
                    break;
            }
        }
    }

    // UI updates

    bool hasChanged = FloorObject::applyAllChanges();
    if (hasChanged) {
        Map::updateMinimap();
    }

    return true;
}

void StateRaceDemo::draw(sf::RenderTarget& window) {
    // scale
    static constexpr const float NORMAL_WIDTH = 512.0f;
    const float scale = window.getSize().x / NORMAL_WIDTH;
    sf::Vector2u windowSize = game.getWindow().getSize();
    const float PAD_TOP = Map::SKY_HEIGHT_PCT * 1.5f * windowSize.y;

    window.clear(sf::Color::Black);

    // Get textures from map
    sf::Texture tSkyBack, tSkyFront, tCircuit;
    Map::skyTextures(pseudoPlayer, tSkyBack, tSkyFront);
    Map::circuitTexture(pseudoPlayer, tCircuit);

    // Create sprites and scale them accordingly
    sf::Sprite skyBack(tSkyBack), skyFront(tSkyFront), circuit(tCircuit);
    float backFactor = windowSize.x / (float)tSkyBack.getSize().x;
    float frontFactor = windowSize.x / (float)tSkyFront.getSize().x;
    skyBack.setScale(backFactor, backFactor);
    skyFront.setScale(frontFactor, frontFactor);

    // Sort them correctly in Y position and draw
    float currentHeight = PAD_TOP;
    skyBack.setPosition(0.0f, currentHeight);
    skyFront.setPosition(0.0f, currentHeight);
    window.draw(skyBack);
    window.draw(skyFront);
    currentHeight += windowSize.y * Map::SKY_HEIGHT_PCT;
    circuit.setPosition(0.0f, currentHeight);
    window.draw(circuit);
    currentHeight += windowSize.y * Map::CIRCUIT_HEIGHT_PCT;

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
        pair.second->move(0.0f, PAD_TOP);
        window.draw(*pair.second);
    }

    sf::Image black;
    black.create(windowSize.x, windowSize.y, sf::Color::Black);
    sf::Texture blackTex;
    blackTex.loadFromImage(black);

    // credits below
    sf::Sprite blackOverlay(blackTex);
    blackOverlay.setPosition(0.0f, currentHeight);
    window.draw(blackOverlay);
    blackOverlay.scale(1.0f, PAD_TOP / windowSize.y);
    blackOverlay.setPosition(0.0f, 0.0f);
    window.draw(blackOverlay);

    sf::Vector2f textPos = ABS_CREDITS;
    TextUtils::write(
        window, "super mario kart",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    textPos = ABS_NAME0;
    TextUtils::write(
        window, "javier gimenez",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    textPos += REL_NAMEDY;
    TextUtils::write(
        window, "victor martinez",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    textPos += REL_NAMEDY;
    TextUtils::write(
        window, "diego royo",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);
    textPos = ABS_COPY;
    TextUtils::write(
        window, "unizar      2020",
        sf::Vector2f(textPos.x * windowSize.x, textPos.y * windowSize.y),
        scale * 2.0f);

    // black fade
    if (fadeTime < FADE_TIME || fadeFinished) {
        float pct = fadeTime / FADE_TIME;
        if (!raceFinished) {
            pct = 1.0f - pct;
        }
        if (fadeFinished) {
            pct = 1.0f;
        }
        int alpha = std::min(pct * 255.0f, 255.0f);
        sf::Sprite blackSprite(blackTex);
        blackSprite.setPosition(0.0f, 0.0f);
        blackSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(blackSprite);
    }
}