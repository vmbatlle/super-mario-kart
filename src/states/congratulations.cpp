#define _USE_MATH_DEFINES
#include "congratulations.h"

const sf::Time StateCongratulations::TIME_FADE = sf::seconds(2.0f),
               StateCongratulations::TIME_ANIMATION = sf::seconds(2.5f),
               StateCongratulations::TIME_WAIT = sf::seconds(15.0f);
const sf::Vector2f StateCongratulations::CAMERA_DISPLACEMENT =
    sf::Vector2f(0.0f, 0.35f);

const std::array<sf::Vector3f, 3> StateCongratulations::PODIUM_DISPLACEMENTS = {
    sf::Vector3f(-0.05e-2f, 1e-5f, 9.0f), sf::Vector3f(-0.62e-2f, 1e-5f, 6.0f),
    sf::Vector3f(0.5e-2f, 1e-5f, 3.0f)};

void StateCongratulations::init(const GrandPrixRankingArray& standings) {
    currentTime = sf::Time::Zero;

    // load circuit & get podium position using player initial positions
    Map::loadCourse(CIRCUIT_ASSET_NAMES[(uint)circuit]);
    targetCameraPosition =
        (Map::getPlayerInitialPosition(1) + Map::getPlayerInitialPosition(2)) /
        2.0f;
    // items use pixel (up to MAP_ASSETS_WIDTH) coordinates
    Map::addItem(ItemPtr(new Podium(targetCameraPosition)));

    // spawn player, which uses 0-1 coordinates
    targetCameraPosition =
        sf::Vector2f(targetCameraPosition.x / MAP_ASSETS_WIDTH,
                     targetCameraPosition.y / MAP_ASSETS_HEIGHT);
    pseudoPlayer = DriverPtr(new Driver(
        "assets/drivers/invisible.png",
        targetCameraPosition + CAMERA_DISPLACEMENT, M_PI_2 * -1.0f,
        MAP_ASSETS_WIDTH, MAP_ASSETS_HEIGHT, DriverControlType::DISABLED,
        VehicleProperties::GODMODE, MenuPlayer(0)));

    // configure driver array so only first 3 elements matter (last 5 are set
    // to pseudoPlayer so they aren't rendered by getDriverDrawables)
    for (uint i = 0; i < standings.size(); i++) {
        if (standings[i].first->getPj() == player) {
            playerRankedPosition = i + 1;
        }
        if (i < PODIUM_DISPLACEMENTS.size()) {
            Driver* driver = standings[i].first;
            driver->position =
                targetCameraPosition + sf::Vector2f(PODIUM_DISPLACEMENTS[i].x,
                                                    PODIUM_DISPLACEMENTS[i].y);
            driver->height = PODIUM_DISPLACEMENTS[i].z;
            driver->posAngle = M_PI_2;
            driver->controlType = DriverControlType::DISABLED;
            orderedDrivers[i] = DriverPtr(driver, [](Driver*) {});
        } else {
            orderedDrivers[i] = pseudoPlayer;
        }
    }
}

void StateCongratulations::fixedUpdate(const sf::Time& deltaTime) {
    currentTime += deltaTime;
    float BEFORE = 0.02f;  // move camera a bit behind the podium
    float displacementPct = BEFORE;
    if (currentTime < TIME_ANIMATION) {
        displacementPct = 1.0f + BEFORE - currentTime / TIME_ANIMATION;
    }
    pseudoPlayer->position =
        targetCameraPosition + CAMERA_DISPLACEMENT * displacementPct;
    if (currentTime > TIME_WAIT) {
        game.popState();
    }
}

void StateCongratulations::draw(sf::RenderTarget& window) {
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
    Map::getItemDrawables(window, pseudoPlayer, scale, wallObjects);  // podium
    Map::getDriverDrawables(window, pseudoPlayer, orderedDrivers, scale,
                            wallObjects);
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
    for (uint i = 0; i < PODIUM_DISPLACEMENTS.size(); i++) {
        Driver* driver = orderedDrivers[i].get();
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

    // UI overlay (text)
    std::string message;
    // all messages should have the same width
    switch (playerRankedPosition) {
        case 1:
            message = "congratulations! you won!";
            break;
        case 2:
        case 3:
            message = " good job! try for 1st!  ";
            break;
        default:
            message = " better luck next time   ";
            break;
    }
    float textScale = window.getSize().x / BACKGROUND_WIDTH;
    TextUtils::write(window, message,
                     sf::Vector2f(24.0f * textScale, 14.0f * textScale),
                     textScale);

    // fade to black if necessary
    if (currentTime < TIME_FADE) {
        float pct = 1.0f - currentTime / TIME_FADE;
        int alpha = std::min(pct * 255.0f, 255.0f);
        sf::Image black;
        black.create(windowSize.x, windowSize.y, sf::Color::Black);
        sf::Texture blackTex;
        blackTex.loadFromImage(black);
        sf::Sprite blackSprite(blackTex);
        blackSprite.setPosition(0.0f, 0.0f);
        blackSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(blackSprite);
    }
}