#include "race.h"

void StateRace::init() {
    StateRace::currentTime = sf::seconds(0);
    playerCps.resize(Map::numCheckpoints());
    std::fill(playerCps.begin(), playerCps.end(), false);
    Gui::setWindowSize(game.getWindow().getSize());
    Map::startCourse();

    ranking.resize((int)MenuPlayer::__COUNT);
    for (DriverPtr& driver : drivers) {
        ranking[(int)driver->getPj()] = std::make_pair(driver, 0);
    }
}

void StateRace::usePowerUps(bool front, DriverPtr p) {
    if (p->getPowerUp() != PowerUps::NONE) {
            // Clean item
            p->pickUpPowerUp(PowerUps::NONE);
            if (p->controlType == DriverControlType::PLAYER)
                Gui::setPowerUp(PowerUps::NONE);

            // Throw
            switch(p->getPowerUp()) {
                case PowerUps::BANANA:
                    if (front) {
                        
                    } else {

                    }
                    break;

                case PowerUps::COIN:
                    p->addCoin(10);
                    break;

                case PowerUps::GREEN_SHELL:
                    if (front) {
                        
                    } else {

                    }

                    break;

                case PowerUps::MUSHROOM:
                    p->setBonnusSpeed(1.5);

                    break;
                case PowerUps::RED_SHELL:

                    break;

                case PowerUps::STAR:

                    break;

                case PowerUps::THUNDER:

                    break;

                case PowerUps::NONE:

                    break;
            }

        }
}

void StateRace::handleEvent(const sf::Event& event) {
    if (Input::pressed(Key::ITEM_FRONT, event)) {
        usePowerUps(true, player);
    }
    if (Input::pressed(Key::ITEM_BACK, event)) {
        usePowerUps(false, player);
    }
}

bool sortbysec(const std::pair<DriverPtr,int> &a, 
              const std::pair<DriverPtr,int> &b) 
{ 
    if (a.second < b.second) {
        return true;
    } else if (a.second == b.second) {
        return a.first->getLaps() > b.first->getLaps();
    } else  {
        return false;
    }
} 

void StateRace::fixedUpdate(const sf::Time& deltaTime) {
    // update global time
    currentTime += deltaTime;

    // Map object updates
    Map::updateObjects(deltaTime);
    for (DriverPtr& driver : drivers) {
        // Player position updates
        driver->update(deltaTime);
    }

    // Collision updates
    // Register all objects for fast detection
    CollisionHashMap::resetDynamic();
    for (const DriverPtr& driver : drivers) {
        CollisionHashMap::registerDynamic(driver);
    }

    // Detect collisions with player
    CollisionData data;
    for (const DriverPtr &driver : drivers) {
        if (CollisionHashMap::collide(driver, data)) {
            driver->collisionMomentum = data.momentum;
            driver->speedForward *= data.speedFactor;
            driver->speedTurn *= data.speedFactor;
        }
    }

    // TODO handle collisions with the rest of entities

    // Now that players are updated, check map/etc
    checkpointUpdate();

    // Ranking
    int i = 0;
    for (DriverPtr& driver : drivers) {
        // Player position updates
        driver->update(deltaTime);
        sf::Vector2f pos = driver->position;
        pos = sf::Vector2f(pos.x * MAP_TILES_WIDTH, pos.y * MAP_TILES_HEIGHT);
        ranking[(int)driver->getPj()] = std::make_pair(driver, AIGradientDescent::getGradientValue(pos.x, pos.y));
        i++;
    }
    std::sort(ranking.begin(), ranking.end(), sortbysec); 
    for (int i = 0; i < (int)MenuPlayer::__COUNT; i++) {
        rank[i] = (int)ranking[i].first->getPj();
        ranking[i].first->setRank(i+1);
    }
    Gui::setRanking(player->getRank());


    // Goal condition
    if (playerPassedCps >= Map::numCheckpoints() &&
        Map::inGoal(player->position)) {
        player->rounds++;
        playerPassedCps = 0;
        std::fill(playerCps.begin(), playerCps.end(), false);
        player->addLap();
        Lakitu::showLap(player->getLaps());

        player->controlType = DriverControlType::AI_GRADIENT;
        Lakitu::showFinish();
        game.popState(); // TODO temporal
    }
    Lakitu::update(deltaTime);
    bool hasChanged = FloorObject::applyAllChanges();
    if (hasChanged) {
        Map::updateMinimap();
    }

    // Gui updates
    Gui::update(deltaTime);
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
    Map::getWallDrawables(window, player, wallObjects);
    Map::getDriverDrawables(window, player, drivers, wallObjects);
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
    std::sort(drivers.begin(), drivers.end(),
              [](const DriverPtr& lhs, const DriverPtr& rhs) {
                  return lhs->position.y < rhs->position.y;
              });
    for (const DriverPtr& driver : drivers) {
        sf::Sprite miniDriver = driver->animator.getMinimapSprite(
            driver->posAngle + driver->speedTurn * 0.5f);
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

    //DEBUG
    // sf::Text text;
    // sf::Font font;
    // if (!font.loadFromFile("arial.ttf")) std::cout << "ERROR" << std::endl;
    // text.setFont(font); // font is a sf::Font
    // std::string s = "Positions\n";
    // constexpr int count = (int)MenuPlayer::__COUNT;
    // for (int i = 0; i < 7; i++) {
    //     s = s + std::to_string(rank[i]) + " \n";
    // }
    // text.setString(s);
    // text.setCharacterSize(24); // in pixels, not points!
    // text.setFillColor(sf::Color::Red);
    // text.setStyle(sf::Text::Bold);
    // window.draw(text);
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